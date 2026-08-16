#pragma once
#include <jni.h>

class Menu {
public:
    static bool isOpen;
    static void InitMenu();
    static void RenderUI();
    static bool CheckYButtonInput();
    static void UpdateHandPosition();
};
