#include "Menu.h"
#include <android/log.h>
#include <GLES2/gl2.h>

#define TAG "TvMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

bool Menu::isOpen = false;
bool Menu::notificationsEnabled = true;

// Hand and Pointer Dot Coordinates
static float handPosX = 0.0f, handPosY = 0.0f, handPosZ = 0.0f;
static float dotPosX = 0.0f, dotPosY = 0.0f, dotPosZ = 0.0f;

// Custom Color structure
struct Color {
    float r, g, b, a;
};

// Blood Red Theme Color Definition
static Color bloodRedActive = {0.85f, 0.0f, 0.0f, 1.0f};
static Color bloodRedHover = {1.0f, 0.1f, 0.1f, 1.0f};
static Color bloodRedBg = {0.15f, 0.0f, 0.0f, 0.9f};

void Menu::InitMenu() {
    LOGI("TvMenu UI System Initialized with Blood Red Theme and Pointer Dot.");
}

void Menu::ShowNotification(std::string message) {
    if (notificationsEnabled) {
        LOGI("[NOTIFICATION] %s", message.c_str());
    }
}

bool Menu::CheckYButtonInput() {
    bool isHoldingY = false; // Replace with your actual controller input query for holding Y

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
        // Hand coordinates
        handPosX = 0.1f;
        handPosY = -0.05f;
        handPosZ = 0.3f;

        // Position the pointer dot slightly above the hand for UI selection
        dotPosX = handPosX;
        dotPosY = handPosY + 0.12f; // Offset upward above the hand
        dotPosZ = handPosZ;
    }
}

void Menu::CheckPointerInteraction() {
    if (!isOpen) return;

    // Check if user presses the trigger/activation button while the dot is aligned with a mod button
    bool isTriggerPressed = false; // Replace with your controller trigger/action button query
    
    if (isTriggerPressed) {
        // Logic to check collision between (dotPosX, dotPosY, dotPosZ) and menu buttons
        LOGI("Pointer dot interacted with menu at X: %f, Y: %f", dotPosX, dotPosY);
        ShowNotification("Mod Toggled!");
    }
}

void Menu::RenderUI() {
    CheckYButtonInput();
    UpdateHandPosition();
    CheckPointerInteraction();

    if (!isOpen) return;
    
    // Render code for menu and the pointer dot at (dotPosX, dotPosY, dotPosZ)
}
