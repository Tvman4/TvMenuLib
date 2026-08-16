#include "Menu.h"
#include <android/log.h>
#include <GLES2/gl2.h>

#define TAG "TvMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

bool Menu::isOpen = false;
bool Menu::notificationsEnabled = true;

static float handPosX = 0.0f, handPosY = 0.0f, handPosZ = 0.0f;
static bool yButtonPreviouslyPressed = false;

void Menu::InitMenu() {
    LOGI("TvMenu UI System Initialized.");
}

void Menu::ShowNotification(std::string message) {
    if (notificationsEnabled) {
        LOGI("[NOTIFICATION] %s", message.c_str());
        // In a full ImGui setup, this pushes a toast notification onto the rendered hand overlay screen.
    }
}

bool Menu::CheckYButtonInput() {
    bool currentYState = false; // Replace with actual controller query
    if (currentYState && !yButtonPreviouslyPressed) {
        isOpen = !isOpen;
        ShowNotification(isOpen ? "Menu Opened" : "Menu Closed");
    }
    yButtonPreviouslyPressed = currentYState;
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
