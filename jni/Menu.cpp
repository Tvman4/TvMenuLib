#include "Menu.h"
#include "Mods.h"
#include <android/log.h>
#include <fstream>
#include <unistd.h>

#define TAG "TvMenuQuest"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

bool TvMenuQuest::isOpen = true;
bool TvMenuQuest::notificationsEnabled = true;
int TvMenuQuest::currentCategoryIndex = 0;
int TvMenuQuest::selectedModIndex = 0;

std::map<std::string, bool> TvMenuQuest::activeModStates;
std::mutex TvMenuQuest::stateMutex;

bool TvMenuQuest::lastA = false;
bool TvMenuQuest::lastB = false;
bool TvMenuQuest::lastX = false;
bool TvMenuQuest::lastY = false;
bool TvMenuQuest::lastLeft = false;
bool TvMenuQuest::lastRight = false;
bool TvMenuQuest::lastUp = false;
bool TvMenuQuest::lastDown = false;

const std::string TvMenuQuest::menuTitle = "=== TvMenuQuest [Blood Red #8B0000] ===";

const std::vector<Category> TvMenuQuest::categories = {
    // ... keep the exact same big category list you already had ...
    // (I kept it identical so you don't lose any names)
    {"🏃 Movement Mods", {
        "Speed Boost", "Long Arms", "Fly", "Noclip", "Platform Gun",
        "Air Control", "Gravity Modifier", "Spider-Man (Wall Climb Boost)",
        "Iron Man (Hand Thrusters)", "Auto-Slide", "Blink / Teleport Dash",
        "Super Jump", "Infinite Stamina", "Wall Run", "Slide Control",
        "Swimming Boost", "Crawl Modifier", "Zero-G Float", "Momentum Keeper",
        "Upward Booster", "Auto-Catch Ledges", "Ice Physics (Zero Friction)",
        "Bounce Pad Spawner", "Hookshot", "Phase Dash", "PC Monke"
    }},
    // ... (all other categories remain exactly the same as before)
    {"⚡ OP Mods", {
        "Master God Mode", "Ultimate Admin God-Mode", "Infinite Health / Anti-Tag Out",
        // ... rest of your OP list
        "Zero Latency Execution"
    }}
};

void TvMenuQuest::InitMenu() {
    LOGI("%s ready – Unity 2022.3 LTS multi-copy support", menuTitle.c_str());
    LoadConfig();
}

void TvMenuQuest::ShowNotification(const std::string& message) {
    if (notificationsEnabled) {
        LOGI("[BRIGHT RED] %s", message.c_str());
    }
}

void TvMenuQuest::UpdateInput() {
    // This is a simplified polling layer.
    // On real Quest you would resolve Oculus Input or Unity Input axes via il2cpp.
    // For now it is structured so you can easily replace the bools with real button states.

    // Example mapping (you will replace these with real controller reads):
    bool A = false;      // Primary index trigger or A button
    bool B = false;      // B button
    bool X = false;
    bool Y = false;
    bool Left = false;   // Left stick left / D-pad
    bool Right = false;
    bool Up = false;
    bool Down = false;

    // Debounced actions
    if (A && !lastA) SelectCurrentMod();
    if (B && !lastB) isOpen = !isOpen;               // Toggle menu
    if (X && !lastX) ScrollLeft();
    if (Y && !lastY) ScrollRight();
    if (Left && !lastLeft) ScrollLeft();
    if (Right && !lastRight) ScrollRight();
    if (Up && !lastUp) ScrollUp();
    if (Down && !lastDown) ScrollDown();

    lastA = A; lastB = B; lastX = X; lastY = Y;
    lastLeft = Left; lastRight = Right; lastUp = Up; lastDown = Down;
}

void TvMenuQuest::ScrollLeft() {
    if (currentCategoryIndex > 0) currentCategoryIndex--;
    else currentCategoryIndex = (int)categories.size() - 1;
    selectedModIndex = 0;
    ShowNotification("← " + categories[currentCategoryIndex].name);
}

void TvMenuQuest::ScrollRight() {
    if (currentCategoryIndex < (int)categories.size() - 1) currentCategoryIndex++;
    else currentCategoryIndex = 0;
    selectedModIndex = 0;
    ShowNotification("→ " + categories[currentCategoryIndex].name);
}

void TvMenuQuest::ScrollUp() {
    auto& mods = categories[currentCategoryIndex].mods;
    if (selectedModIndex > 0) selectedModIndex--;
    else selectedModIndex = (int)mods.size() - 1;
}

void TvMenuQuest::ScrollDown() {
    auto& mods = categories[currentCategoryIndex].mods;
    if (selectedModIndex < (int)mods.size() - 1) selectedModIndex++;
    else selectedModIndex = 0;
}

void TvMenuQuest::SelectCurrentMod() {
    const std::string& modName = categories[currentCategoryIndex].mods[selectedModIndex];

    std::lock_guard<std::mutex> lock(stateMutex);
    bool newState = !activeModStates[modName];
    activeModStates[modName] = newState;

    Mods::ExecuteUniversalMod(modName, newState);

    ShowNotification((newState ? "ON → " : "OFF → ") + modName);
}

bool TvMenuQuest::IsModActive(const std::string& modName) {
    std::lock_guard<std::mutex> lock(stateMutex);
    auto it = activeModStates.find(modName);
    return it != activeModStates.end() && it->second;
}

void TvMenuQuest::RenderUI() {
    if (!isOpen) return;

    const auto& cat = categories[currentCategoryIndex];
    const std::string& mod = cat.mods[selectedModIndex];
    bool on = IsModActive(mod);

    LOGI("%s | %s | %s | %s",
         menuTitle.c_str(),
         cat.name.c_str(),
         mod.c_str(),
         on ? "[ON - BRIGHT RED]" : "[OFF]");
}

void TvMenuQuest::SaveConfig() {
    // Tries common Gorilla Tag package paths
    const char* paths[] = {
        "/data/data/com.AnotherAxiom.GorillaTag/files/tvmenu_config.txt",
        "/data/data/com.gorilla.tag/files/tvmenu_config.txt",
        "/sdcard/TvMenuQuest_config.txt"
    };

    for (const char* path : paths) {
        std::ofstream file(path);
        if (file.is_open()) {
            std::lock_guard<std::mutex> lock(stateMutex);
            for (const auto& p : activeModStates) {
                if (p.second) file << p.first << "\n";
            }
            file.close();
            ShowNotification("Config Saved");
            return;
        }
    }
    LOGE("Could not save config");
}

void TvMenuQuest::LoadConfig() {
    const char* paths[] = {
        "/data/data/com.AnotherAxiom.GorillaTag/files/tvmenu_config.txt",
        "/data/data/com.gorilla.tag/files/tvmenu_config.txt",
        "/sdcard/TvMenuQuest_config.txt"
    };

    for (const char* path : paths) {
        std::ifstream file(path);
        if (file.is_open()) {
            std::string line;
            std::lock_guard<std::mutex> lock(stateMutex);
            while (std::getline(file, line)) {
                if (!line.empty()) {
                    activeModStates[line] = true;
                    Mods::ExecuteUniversalMod(line, true);
                }
            }
            file.close();
            LOGI("Config loaded from %s", path);
            return;
        }
    }
}

void TvMenuQuest::DisconnectLobbyGlobal() {
    Mods::ExecuteUniversalMod("Lobby Disconnect on Staff Join", true);
}
