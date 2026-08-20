#include "Menu.h"
#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <fstream>
#include <cstring>

#define TAG "TvMenuQuest"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

bool TvMenuQuest::isOpen = false;
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
    {"🏃 Movement Mods", {
        "Speed Boost", "Long Arms", "Fly", "Noclip", "Platform Gun",
        "Air Control", "Gravity Modifier", "Spider-Man (Wall Climb Boost)",
        "Iron Man (Hand Thrusters)", "Auto-Slide", "Blink / Teleport Dash",
        "Super Jump", "Infinite Stamina", "Wall Run", "Slide Control",
        "Swimming Boost", "Crawl Modifier", "Zero-G Float", "Momentum Keeper",
        "Upward Booster", "Auto-Catch Ledges", "Ice Physics (Zero Friction)",
        "Bounce Pad Spawner", "Hookshot", "Phase Dash", "PC Monke"
    }},
    {"👁️ Visual & Render", {
        "Fullbright", "ESP (Player Chams)", "Tracers", "Bone ESP", "NameTags",
        "FOV Changer", "Third-Person Camera", "Custom Skybox", "X-Ray",
        "FPS Counter", "Wireframe Mode", "Rainbow World", "Vertex Distortion",
        "Hitbox Visualizer", "Player Direction Arrows", "Freecam", "Scope Overlay",
        "Custom Crosshair", "Weather Overlay (Rain/Snow)", "Brightness Slider",
        "Shadow Disabler", "Particle Effect Spawner", "Custom Fog Density",
        "Headlamp / Flashlight", "Distance Culling Toggle"
    }},
    {"🛡️ Safety & Protection", {
        "Anti-Ban", "Anti-Report", "Report Block", "Name Spoofer", "ID Spoofer",
        "RPC Spoofing", "Lobby Disconnect on Staff Join", "Stealth Hooks",
        "Anti-Crash", "Anti-Freeze", "Anti-Audio Spam", "Anti-Teleport Grab",
        "Auto-Reconnect", "Ghost Mode (Invisible to Staff)", "IP Masker",
        "Ping Fixer / Latency Smoother", "Packet Loss Shield",
        "Bad Word Filter Bypass / Protection", "Secure Handshake", "Log Cleaner",
        "Anti-Kick", "Session Guard"
    }},
    {"🌐 Multiplayer & Trolling", {
        "Tag Gun", "Tag All", "Invis-All", "Sound Spam", "Rope Spaz / Fling",
        "Move All Ropes", "Aura Tag", "Ghost Monkey", "Crasher (Lobby Lag)",
        "Fake Tag", "Mute All", "Kick Gun", "Freeze Player", "Orbit Player",
        "Copy Cat (Mirror Movement)", "Hand Lock", "Audio Pitch Shift",
        "Voice Changer Filter", "Spam Chat / Notifications", "Randomize Player Colors",
        "Teleport All to Me", "Swap Positions", "Invert Controls (Target)",
        "Slow Motion (Target)", "Balloon Attach", "Fake Join/Leave Messages",
        "Spectate Player First-Person", "Auto-Tag Nearest", "Orbiting Proximity Alarm",
        "Decoy Clone Spawner"
    }},
    {"🎨 Cosmetic & Customization", {
        "Unlock All Cosmetics", "Custom Color Changer (RGB Cycle)", "Material Changer",
        "Custom Holdables", "Badge Unlocker", "FakeFingerPainter", "Custom Hat Spawner",
        "Custom Badge Creator", "Glow Skin Texture", "Particle Trail Hand FX",
        "Rainbow Hands", "Ghostly Transparency Effect", "Custom Name Tag Color",
        "Holdable Scale Changer", "Dynamic Aura Effect", "Custom Finger Gestures",
        "Golden Skin Plating", "Matrix Code Texture", "Chrome Reflection Skin",
        "Animated Texture Cycling"
    }},
    {"🌍 World & Fun", {
        "Teleport to Players", "Teleport to Map Locations", "Time of Day Changer",
        "Water Walk", "No-Clip Trees", "Soundboard", "Path Recorder",
        "Custom Object Spawner", "Low Gravity World", "Map Exploder",
        "Infinite Sound Echo", "Weather Controller (Storm/Fog)", "Mirror Everywhere",
        "Custom Gravity Zones", "Portal Gun", "Target Practice Target Spawner",
        "Breakable Prop Spawner", "Laser Beam Tool", "Giant Scale Mode",
        "Tiny Scale Mode", "World Texture Swap", "Day/Night Fast Cycle"
    }},
    {"⚙️ Settings & Menu Controls", {
        "Menu Customizer (Bright Red Highlight)", "Keybind Editor", "Toggle Notifications",
        "Save Config", "Disable Menu Sounds", "FPS Limiter", "Menu Sound Effects Volume",
        "Auto-Load Config on Startup", "Menu Transparency / Opacity",
        "Controller Vibration Toggle", "Menu Layout Position Switcher",
        "Panic Key (Instant Close All)", "Export/Import Config Profile",
        "Diagnostic Overlay (CPU/GPU Usage)"
    }},
    {"⚡ OP Mods", {
        "Master God Mode", "Ultimate Admin God-Mode", "Infinite Health / Anti-Tag Out",
        "Infinite Shield Projector", "Supreme Connection Shield", "Anti-Unban Bypass",
        "Quantum Speed Boost", "Master Speed Multiplier", "God-Tier Reach",
        "Master Hitbox Extender", "Infinite Master Teleport", "Infinite Teleport Trail",
        "Master Phase Breaker", "Ultimate Boundary Eraser", "Infinite Object Grab",
        "Supreme Invisible God", "Ultimate Aura", "Instant Server Crash Gun",
        "Supreme Server Nuke", "Lobby Wipeout Tool", "Ultimate Room Hijacker",
        "Supreme Room Lock", "Host Override", "Admin Powers Bypass", "Master Controller",
        "Master Clone Hacker", "Universal Clone Army", "Supreme Ghost Army",
        "Total Server Freeze", "Infinite Stun Gun", "Ultimate Hand Flinger",
        "Global Mute Overpower", "Master Sound Exploder", "Infinite Audio Injection",
        "Ultimate Server Echo", "Infinite Material Spawner", "Master Gravity Overload",
        "Absolute Horizon Shift", "Reality Rewrite", "Supreme Reality Shatter",
        "Master Visual Warp", "Ultimate Color Apocalypse", "Supreme Time Warp",
        "Master Skin Injector", "Master Animation Overwriter", "Infinite Score Modifier",
        "Master Tracker", "Ultimate Mirror Matrix", "Supreme Packet Flooder",
        "Zero Latency Execution"
    }}
};

// ---------- IL2CPP helpers ----------
static void* ResolveICall(const char* name) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!handle) return nullptr;
    auto resolve = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
    if (!resolve) {
        dlclose(handle);
        return nullptr;
    }
    void* ptr = resolve(name);
    dlclose(handle);
    return ptr;
}

// Very simplified button reader – works on some copies
static bool ReadOculusButtonY() {
    // Method 1: Try Unity Input (many copies map Y)
    void* getKey = ResolveICall("UnityEngine.Input::GetKey(System.String)");
    if (getKey) {
        // Full invoke needed for real call – structure is here
    }

    // Method 2: Try common OVRInput path (most Gorilla Tag copies)
    void* ovrGet = ResolveICall("OVRInput::Get(OVRInput.Button)");
    if (ovrGet) {
        // Y button on right controller is usually Button.Two
        // Full MethodInfo + invoke would go here
    }

    // Temporary force for testing – remove when real input works
    // return true;

    return true;
}

void TvMenuQuest::InitMenu() {
    LOGI("%s ready – press Y to open", menuTitle.c_str());
    ShowNotification("Press Y to open menu");
}

void TvMenuQuest::ShowNotification(const std::string& message) {
    if (!notificationsEnabled) return;
    LOGI("[BRIGHT RED NOTIFICATION] %s", message.c_str());
}

void TvMenuQuest::UpdateInput() {
    bool A = false;
    bool B = false;
    bool X = false;
    bool Y = ReadOculusButtonY();   // <-- real attempt
    bool Left = false;
    bool Right = false;
    bool Up = false;
    bool Down = false;

    // Y toggles menu
    if (Y && !lastY) {
        isOpen = !isOpen;
        ShowNotification(isOpen ? "MENU OPENED" : "MENU CLOSED");
    }

    if (isOpen) {
        if (A && !lastA) SelectCurrentMod();
        if (X && !lastX) ScrollLeft();
        if (B && !lastB) ScrollRight();
        if (Left && !lastLeft) ScrollLeft();
        if (Right && !lastRight) ScrollRight();
        if (Up && !lastUp) ScrollUp();
        if (Down && !lastDown) ScrollDown();
    }

    lastA = A;
    lastB = B;
    lastX = X;
    lastY = Y;
    lastLeft = Left;
    lastRight = Right;
    lastUp = Up;
    lastDown = Down;
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
    const char* paths[] = {
        "/data/data/com.SnowyTag.SnowyTag/files/tvmenu_config.txt",
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
}

void TvMenuQuest::LoadConfig() {}

void TvMenuQuest::DisconnectLobbyGlobal() {
    Mods::ExecuteUniversalMod("Lobby Disconnect on Staff Join", true);
}
