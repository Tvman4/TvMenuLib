#pragma once
#include <string>
#include <vector>

struct Category {
    std::string name;
    std::vector<std::string> mods;
};

class TvMenuQuest {
public:
    static bool isOpen;
    static int currentCategory;
    static int currentMod;

    static const std::vector<Category> categories;

    static void Init();
    static void Draw();
    static void Toggle();
};
