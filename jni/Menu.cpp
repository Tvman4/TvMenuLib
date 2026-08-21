#include "Menu.h"
#include "Mods.h"
#include "imgui.h"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TvMenuQuest", __VA_ARGS__)

bool TvMenuQuest::isOpen = true;
int TvMenuQuest::currentCategory = 0;
bool TvMenuQuest::roundedCorners = true;
float TvMenuQuest::menuAlpha = 0.92f;
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
        "Rounded Corners", "Menu Opacity"
    }}
};

void TvMenuQuest::Init() {
    LOGI("TvMenuQuest Init");
}

void TvMenuQuest::Draw() {
    if (!isOpen) return;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = roundedCorners ? 10.0f : 0.0f;
    style.FrameRounding  = roundedCorners ? 6.0f : 0.0f;
    style.WindowBorderSize = 1.0f;

    // Blood red theme
    ImGui::PushStyleColor(ImGuiCol_WindowBg,        ImVec4(0.55f, 0.0f, 0.0f, menuAlpha));
    ImGui::PushStyleColor(ImGuiCol_TitleBg,         ImVec4(0.40f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive,   ImVec4(0.60f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.70f, 0.05f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(0.90f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(1.00f, 0.25f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text,            ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Separator,       ImVec4(1.0f, 0.3f, 0.3f, 0.5f));

    ImGui::SetNextWindowSize(ImVec2(460, 520), ImGuiCond_FirstUseEver);
    ImGui::Begin("TvMenuQuest", &isOpen, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("TvMenuQuest");
    ImGui::Separator();

    // Category arrows
    if (ImGui::Button("<")) {
        currentCategory--;
        if (currentCategory < 0) currentCategory = (int)categories.size() - 1;
    }
    ImGui::SameLine();
    ImGui::Text("%s", categories[currentCategory].name.c_str());
    ImGui::SameLine();
    if (ImGui::Button(">")) {
        currentCategory++;
        if (currentCategory >= (int)categories.size()) currentCategory = 0;
    }

    ImGui::Separator();

    auto& mods = categories[currentCategory].mods;

    if (categories[currentCategory].name == "Settings") {
        ImGui::Checkbox("Rounded Corners", &roundedCorners);
        ImGui::SliderFloat("Menu Opacity", &menuAlpha, 0.4f, 1.0f);
    } else {
        for (auto& mod : mods) {
            bool enabled = modStates[mod];
            std::string label = mod + (enabled ? "   [ON]" : "   [OFF]");

            if (ImGui::Button(label.c_str(), ImVec2(380, 36))) {
                modStates[mod] = !enabled;
                Mods::ExecuteUniversalMod(mod, modStates[mod]);
            }
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Disconnect from Lobby", ImVec2(380, 42))) {
        Mods::ExecuteUniversalMod("Disconnect", true);
    }

    ImGui::End();
    ImGui::PopStyleColor(8);
}
