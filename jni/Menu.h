#pragma once
#include <string>
#include <vector>
#include <map>
#include <mutex>

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
    static void UpdateInput();
    static void ScrollLeft();
    static void ScrollRight();
    static void ScrollUp();
    static void ScrollDown();
    static void SelectCurrentMod();
    static void ShowNotification(const std::string& message);
    static void DisconnectLobbyGlobal();
    static void SaveConfig();
    static void LoadConfig();
    static bool IsModActive(const std::string& modName);

private:
    static std::map<std::string, bool> activeModStates;
    static std::mutex stateMutex;

    static bool lastA, lastB, lastX, lastY;
    static bool lastLeft, lastRight, lastUp, lastDown;
};
