#pragma once
#include <jni.h>
#include <string>
#include <vector>

struct Category {
    std::string name;
    std::vector<std::string> mods;
};

class TvMenuQuest {
public:
    static bool isOpen;
    static bool notificationsEnabled;
    static int currentCategoryIndex;
    static int selectedModIndex;
    static const std::string menuTitle;
    static const std::vector<Category> categories;

    static void InitMenu();
    static void RenderUI();
    static void ScrollLeft();
    static void ScrollRight();
    static void SelectCurrentMod();
    static void ShowNotification(std::string message);
    static void DisconnectLobbyGlobal();
};
