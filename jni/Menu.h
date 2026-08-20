#pragma once
#include <string>
#include <vector>
#include <map>

struct Category {
    std::string name;
    std::vector<std::string> mods;
};

class TvMenuQuest {
public:
    static bool isOpen;
    static int currentCategory;
    static int currentMod;

    // Settings
    static bool roundedCorners;
    static float menuAlpha;
    static bool showBackground;

    static const std::vector<Category> categories;
    static std::map<std::string, bool> modStates;

    static void Init();
    static void Draw();
    static void Toggle();
};
