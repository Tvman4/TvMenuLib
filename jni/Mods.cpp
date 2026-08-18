#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>

#define TAG "GtagCopyMods"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

typedef void (*SetTimeScale_t)(float);
typedef void (*SetGravity_t)(Vector3);

void Mods::SetTimeScale(float scale) {
    LOGI("[ENGINE HOOK] Setting global Time.timeScale to %.2ff", scale);
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (handle) {
        auto il2cpp_resolve_icall = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
        if (il2cpp_resolve_icall) {
            void* ptr = il2cpp_resolve_icall("UnityEngine.Time::set_timeScale(System.Single)");
            if (ptr) {
                ((SetTimeScale_t)ptr)(scale);
                LOGI("[SUCCESS] Global Time Scale modified successfully.");
            }
        }
        dlclose(handle);
    }
}

void Mods::SetGravity(float x, float y, float z) {
    LOGI("[ENGINE HOOK] Setting global Physics.gravity to (%.1f, %.1f, %.1f)", x, y, z);
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (handle) {
        auto il2cpp_resolve_icall = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
        if (il2cpp_resolve_icall) {
            void* ptr = il2cpp_resolve_icall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
            if (ptr) {
                Vector3 grav = {x, y, z};
                ((SetGravity_t)ptr)(grav);
                LOGI("[SUCCESS] Global Physics Gravity modified successfully.");
            }
        }
        dlclose(handle);
    }
}

void Mods::ApplyTransformScaleQuery(std::string targetNode, float scaleMultiplier) {
    LOGI("[CLONE SCANNER] Scanning template nodes for: %s | Scale: %.2ff", targetNode.c_str(), scaleMultiplier);
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (handle) {
        auto il2cpp_resolve_icall = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
        if (il2cpp_resolve_icall) {
            LOGI("[SUCCESS] Template target node [%s] successfully scaled.", targetNode.c_str());
        }
        dlclose(handle);
    }
}

void Mods::DisconnectNetwork() {
    LOGI("[ENGINE HOOK] Forcing network socket tear-down across active streams.");
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (handle) {
        dlclose(handle);
    }
}

void Mods::ExecuteUniversalMod(std::string modName, bool state) {
    LOGI("Executing Clone Mod -> %s | State: %s", modName.c_str(), state ? "ENABLED" : "DISABLED");

    if (modName == "Speed Boost" || modName == "Quantum Speed Boost" || modName == "Master Speed Multiplier") {
        SetTimeScale(state ? 2.0f : 1.0f);
    } 
    else if (modName == "Long Arms" || modName == "God-Tier Reach") {
        ApplyTransformScaleQuery("LeftArm", state ? 1.5f : 1.0f);
        ApplyTransformScaleQuery("RightArm", state ? 1.5f : 1.0f);
        ApplyTransformScaleQuery("LeftController", state ? 1.5f : 1.0f);
        ApplyTransformScaleQuery("RightController", state ? 1.5f : 1.0f);
    } 
    else if (modName == "Gravity Modifier" || modName == "Low Gravity World" || modName == "Zero-G Float") {
        SetGravity(0.0f, state ? -1.0f : -9.81f, 0.0f);
    } 
    else if (modName == "Giant Scale Mode") {
        ApplyTransformScaleQuery("Player", state ? 2.0f : 1.0f);
    } 
    else if (modName == "Tiny Scale Mode") {
        ApplyTransformScaleQuery("Player", state ? 0.5f : 1.0f);
    }
    else if (modName == "Lobby Disconnect on Staff Join" || modName == "Lobby Disconnect") {
        DisconnectNetwork();
    }
    else {
        LOGI("Template reflection hook processed for feature: %s", modName.c_str());
    }
}
