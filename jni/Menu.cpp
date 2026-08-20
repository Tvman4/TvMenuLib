#include "Menu.h"
#include "Mods.h"
#include "imgui.h"
#include <android/log.h>
#include <map>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TvMenuQuest", __VA_ARGS__)

bool TvMenuQuest::isOpen = true;
int TvMenuQuest::currentCategory = 0;
int TvMenuQuest::currentMod = 0;

const std::vector<Category> TvMenuQuest::categories = {
    {"Movement", {"Long Arms", "Speed Boost", "Fly", "Noclip", "Gravity"}},
    {"Multiplayer", {"Tag All", "Orbit", "Crasher"}},
    {"Cosmetics", {"Unlock All Cosmetics"}},
    {"OP", {"God Mode", "Master Speed"}}
};

void TvMenuQuest::Init() {
    LOGI("TvMenuQuest initialized");
}

void TvMenuQuest::Toggle() {
    isOpen = !isOpen;
}

void TvMenuQuest::Draw() {
    if (!isOpen) return;

    // Blood red menu + white text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.545f, 0.0f, 0.0f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.40f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.60f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.70f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.85f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::SetNextWindowSize(ImVec2(460, 420), ImGuiCond_FirstUseEver);
    ImGui::Begin("TvMenuQuest", &isOpen, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("TvMenuQuest");
    ImGui::Separator();

    for (int i = 0; i < (int)categories.size(); i++) {
        if (ImGui::Selectable(categories[i].name.c_str(), currentCategory == i)) {
            currentCategory = i;
            currentMod = 0;
        }
    }

    ImGui::Separator();
    ImGui::Text("Mods:");

    auto& mods = categories[currentCategory].mods;
    static std::map<std::string, bool> states;

    for (int i = 0; i < (int)mods.size(); i++) {
        bool selected = (currentMod == i);
        std::string label = mods[i];
        if (states[mods[i]]) label += "  [ON]";

        if (ImGui::Selectable(label.c_str(), selected)) {
            currentMod = i;
            states[mods[i]] = !states[mods[i]];
            Mods::ExecuteUniversalMod(mods[i], states[mods[i]]);
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(7);
}
