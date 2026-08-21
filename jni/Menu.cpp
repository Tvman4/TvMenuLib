#include "Menu.h"
#include "Mods.h"
#include "imgui.h"
#include <android/log.h>
#include <map>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TvMenuQuest", __VA_ARGS__)

bool TvMenuQuest::isOpen = true;
int TvMenuQuest::currentCategory = 0;
int TvMenuQuest::currentMod = 0;

bool TvMenuQuest::roundedCorners = true;
float TvMenuQuest::menuAlpha = 0.95f;
bool TvMenuQuest::showBackground = true;

std::map<std::string, bool> TvMenuQuest::modStates;

const std::vector<Category> TvMenuQuest::categories = {
    {"Movement", {
        "Long Arms", "Speed Boost", "Fly", "Noclip", "Gravity",
        "Super Jump", "Zero Gravity", "Air Jump"
    }},
    {"Multiplayer", {
        "Tag All", "Orbit", "Ghost Monkey", "Invis All"
    }},
    {"Cosmetics", {
        "Unlock All Cosmetics", "Rainbow Monkey"
    }},
    {"Visual", {
        "Fullbright", "ESP", "Tracers"
    }},
    {"OP", {
        "Master Speed"
    }},
    {"Settings", {
        "Rounded Corners", "Menu Opacity", "Toggle Background"
    }}
};

void TvMenuQuest::Init() {
    LOGI("TvMenuQuest initialized");
}

void TvMenuQuest::Toggle() {
    isOpen = !isOpen;
}

void TvMenuQuest::Draw() {
    if (!isOpen) return;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = roundedCorners ? 12.0f : 0.0f;
    style.FrameRounding  = roundedCorners ? 8.0f : 0.0f;
    style.GrabRounding   = roundedCorners ? 8.0f : 0.0f;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.545f, 0.0f, 0.0f, menuAlpha));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.40f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.60f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.70f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.85f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::SetNextWindowSize(ImVec2(500, 560), ImGuiCond_FirstUseEver);
    ImGui::Begin("TvMenuQuest", &isOpen, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("TvMenuQuest");
    ImGui::Separator();

    // Left / Right arrows
    if (ImGui::Button("<")) {
        currentCategory--;
        if (currentCategory < 0) currentCategory = (int)categories.size() - 1;
        currentMod = 0;
    }
    ImGui::SameLine();
    ImGui::Text("%s", categories[currentCategory].name.c_str());
    ImGui::SameLine();
    if (ImGui::Button(">")) {
        currentCategory++;
        if (currentCategory >= (int)categories.size()) currentCategory = 0;
        currentMod = 0;
    }

    ImGui::Separator();
    ImGui::Text("Mods:");

    auto& mods = categories[currentCategory].mods;

    if (categories[currentCategory].name == "Settings") {
        ImGui::Checkbox("Rounded Corners", &roundedCorners);
        ImGui::SliderFloat("Menu Opacity", &menuAlpha, 0.3f, 1.0f);
        ImGui::Checkbox("Show Background", &showBackground);
    } else {
        for (int i = 0; i < (int)mods.size(); i++) {
            bool enabled = modStates[mods[i]];
            std::string label = mods[i] + (enabled ? "  [ON]" : "  [OFF]");

            if (ImGui::Button(label.c_str(), ImVec2(320, 34))) {
                modStates[mods[i]] = !enabled;
                Mods::ExecuteUniversalMod(mods[i], modStates[mods[i]]);
            }
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Disconnect from Lobby", ImVec2(320, 42))) {
        Mods::ExecuteUniversalMod("Disconnect", true);
    }

    ImGui::End();
    ImGui::PopStyleColor(9);
}
