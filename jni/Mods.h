#pragma once
#include <string>

struct Vector3 {
    float x, y, z;
};

class Mods {
public:
    static void ExecuteUniversalMod(const std::string& modName, bool state);

    static void SetTimeScale(float scale);
    static void SetGravity(float x, float y, float z);

    static void ApplyTransformScale(const std::string& targetNode, float scaleMultiplier);
    static void SetLongArms(bool enabled);
    static void SetPlayerScale(float scale);
    static void SetArmScale(float scale);

    static void TagAll(bool enabled);
    static void OrbitPlayers(bool enabled);
    static void Crasher(bool enabled);
    static void UnlockAllCosmetics(bool enabled);

    static void SetFly(bool enabled);
    static void SetNoclip(bool enabled);
    static void SetGodMode(bool enabled);
    static void DisconnectNetwork();
};
