#pragma once
#include <string>

class Mods {
public:
    static void ExecuteUniversalMod(const std::string& modName, bool state);

    static void SetLongArms(bool enabled);
    static void SetSpeed(bool enabled);
    static void SetFly(bool enabled);
    static void SetNoclip(bool enabled);
    static void SetGravity(bool enabled);
    static void TagAll(bool enabled);
    static void Orbit(bool enabled);
    static void Crasher(bool enabled);
    static void UnlockCosmetics(bool enabled);
    static void GodMode(bool enabled);
};
