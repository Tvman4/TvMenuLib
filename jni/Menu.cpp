#include "Menu.h"
#include <android/log.h>
#include <GLES2/gl2.h>

#define TAG "TvMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

bool Menu::isOpen = false;
bool Menu::notificationsEnabled = true;

static float handPosX = 0.0f, handPosY = 0.0f, handPosZ = 0.0f;

// Custom Color structure to avoid needing imgui.h
struct Color {
    float r, g, b, a;
};

// Blood Red Theme Color Definition
static Color bloodRedActive = {0.85f, 0.0f, 0.0f, 1.0f};
static Color bloodRedHover = {1.0f, 0.1f, 0.1f, 1.0f};
static Color bloodRedBg = {0.15f, 0.0f, 0.0f, 0.9f};

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
    // Render code anchored to hand position
}
