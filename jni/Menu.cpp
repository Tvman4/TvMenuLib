#include "Menu.h"
#include "Mods.h"
#include "imgui.h"
#include <android/log.h>

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
        "Tag All", "Orbit", "Crasher", "Ghost Monkey", "Invis All"
    }},
    {"Cosmetics", {
        "Unlock All Cosmetics", "Rainbow Monkey", "Custom Material"
    }},
    {"Visual", {
        "Fullbright", "ESP", "Tracers", "NameTags"
    }},
    {"OP", {
        "God Mode", "Master Speed", "Infinite Reach", "Anti Tag"
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

    // ===== STYLE =====
    ImGuiStyle& style = ImGui::GetStyle();
    if (roundedCorners) {
        style.WindowRounding = 12.0f;
        style.FrameRounding = 8.0f;
        style.GrabRounding = 8.0f;
        style.ScrollbarRounding = 8.0f;
    } else {
        style.WindowRounding = 0.0f;
        style.FrameRounding = 0.0f;
    }

    // Blood red theme + white text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.545f, 0.0f, 0.0f, menuAlpha));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.40f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.60f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.70f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.85f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::SetNextWindowSize(ImVec2(480, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("TvMenuQuest", &isOpen, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("TvMenuQuest");
    ImGui::Separator();

    // ===== CATEGORY BUTTONS =====
    for (int i = 0; i < (int)categories.size(); i++) {
        if (i > 0) ImGui::SameLine();
        bool selected = (currentCategory == i);
        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        }
        if (ImGui::Button(categories[i].name.c_str(), ImVec2(70, 28))) {
            currentCategory = i;
            currentMod = 0;
        }
        if (selected) ImGui::PopStyleColor();
    }

    ImGui::Separator();
    ImGui::Text("Mods:");

    // ===== MODS LIST =====
    if (currentCategory < (int)categories.size()) {
        auto& mods = categories[currentCategory].mods;

        // Special handling for Settings category
        if (categories[currentCategory].name == "Settings") {
            if (ImGui::Checkbox("Rounded Corners", &roundedCorners)) {}
            ImGui::SliderFloat("Menu Opacity", &menuAlpha, 0.3f, 1.0f);
            ImGui::Checkbox("Show Background", &showBackground);
        }
        else {
            for (int i = 0; i < (int)mods.size(); i++) {
                bool enabled = modStates[mods[i]];
                std::string label = mods[i] + (enabled ? "  [ON]" : "  [OFF]");

                if (ImGui::Button(label.c_str(), ImVec2(280, 32))) {
                    modStates[mods[i]] = !enabled;
                    Mods::ExecuteUniversalMod(mods[i], modStates[mods[i]]);
                }
            }
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(9);
}
