#include "Menu.h"
#include <android/log.h>
#include <GLES2/gl2.h>
// Include ImGui header so ImVec4 is recognized
#include "imgui.h" 

#define TAG "TvMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

bool Menu::isOpen = false;
bool Menu::notificationsEnabled = true;

static float handPosX = 0.0f, handPosY = 0.0f, handPosZ = 0.0f;

// Blood Red Theme Color Definition
static ImVec4 bloodRedActive = ImVec4(0.85f, 0.0f, 0.0f, 1.0f);
static ImVec4 bloodRedHover = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
static ImVec4 bloodRedBg = ImVec4(0.15f, 0.0f, 0.0f, 0.9f);

void Menu::InitMenu() {
    LOGI("TvMenu UI System Initialized with Blood Red Theme.");
}

void Menu::ShowNotification(std::string message) {
    if (notificationsEnabled) {
        LOGI("[NOTIFICATION] %s", message.c_str());
    }
}

bool Menu::CheckYButtonInput() {
    bool isHoldingY = false; 

    if (isHoldingY) {
        if (!isOpen) {
            isOpen = true;
            ShowNotification("Menu Opened");
        }
    } else {
        if (isOpen) {
            isOpen = false;
            ShowNotification("Menu Dropped / Closed");
        }
    }
    
    return isOpen;
}

void Menu::UpdateHandPosition() {
    if (isOpen) {
        handPosX = 0.1f;
        handPosY = -0.05f;
        handPosZ = 0.3f;
    }
}

void Menu::RenderUI() {
    CheckYButtonInput();
    UpdateHandPosition();

    if (!isOpen) return;

    // Apply Blood Red Theme Styling
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Button] = bloodRedBg;
    style.Colors[ImGuiCol_ButtonHovered] = bloodRedHover;
    style.Colors[ImGuiCol_ButtonActive] = bloodRedActive;
    style.Colors[ImGuiCol_Header] = bloodRedBg;
    style.Colors[ImGuiCol_HeaderHovered] = bloodRedHover;
    style.Colors[ImGuiCol_HeaderActive] = bloodRedActive;
}
