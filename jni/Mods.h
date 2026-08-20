#pragma once
#include <string>
#include <vector>

struct Vector3 {
    float x, y, z;
};

class Mods {
public:
    static void ExecuteUniversalMod(const std::string& modName, bool state);
    static void SetTimeScale(float scale);
    static void SetGravity(float x, float y, float z);
    static void ApplyTransformScale(const std::string& targetNode, float scaleMultiplier);
    static void DisconnectNetwork();
    static void SetFly(bool enabled);
    static void SetNoclip(bool enabled);
    static void SetGodMode(bool enabled);
};
