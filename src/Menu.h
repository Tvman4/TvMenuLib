#pragma once
#include <string>
#include <vector>
#include <functional>

struct ModDef {
    const char *name;
    const char *category;
    bool toggle = true;          // false = button (fire once)
    std::function<void(bool)> onSet;
    std::function<void()> onTick;
};

class WristMenu {
public:
    static void Init();
    static void Tick();
    static void RegisterAll();
    static std::vector<ModDef> &Mods();
    static bool IsOn(int idx);
    static void SetOn(int idx, bool v);
    static int Count();
};
