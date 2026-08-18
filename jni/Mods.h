#pragma once
#include <string>

struct Vector3 {
    float x, y, z;
};

class Mods {
public:
    static void ExecuteUniversalMod(std::string modName, bool state);
    static void SetTimeScale(float scale);
    static void SetGravity(float x, float y, float z);
    static void ApplyTransformScaleQuery(std::string targetNode, float scaleMultiplier);
    static void DisconnectNetwork();
};
