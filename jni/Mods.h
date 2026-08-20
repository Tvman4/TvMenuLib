#pragma once
#include <string>

struct Vector3 {
    float x, y, z;
};

class Mods {
public:
    static void ExecuteUniversalMod(const std::string& modName, bool state);

    // Core working systems
    static void SetTimeScale(float scale);
    static void SetGravity(float x, float y, float z);
    static void ApplyTransformScale(const std::string& targetNode, float scaleMultiplier);
    static void SetPlayerScale(float scale);
    static void SetArmScale(float scale);

    // Movement
    static void SetFly(bool enabled);
    static void SetNoclip(bool enabled);
    static void SetGodMode(bool enabled);

    // Network
    static void DisconnectNetwork();
};
