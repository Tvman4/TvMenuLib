#pragma once
#include <jni.h>
#include <string>

class Menu {
public:
    static bool isOpen;
    static bool notificationsEnabled;
    static void InitMenu();
    static void RenderUI();
    static bool CheckYButtonInput();
    static void UpdateHandPosition();
    static void ShowNotification(std::string message);
};
