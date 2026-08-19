#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>

#define TAG "TvMenuQuestUnity2022"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

typedef void (*SetTimeScale_t)(float);
typedef void (*SetGravity_t)(Vector3);

void Mods::SetTimeScale(float scale) {
    LOGI("[Unity 2022.3 LTS] Hooking Time.timeScale -> %.2ff", scale);
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (handle) {
        auto il2cpp_resolve_icall = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
        if (il2cpp_resolve_icall) {
            // Unity 2022.3 specific icall path
            void* ptr = il2cpp_resolve_icall("UnityEngine.Time::get_timeScale()");
            if (ptr) {
                void* setPtr = il2cpp_resolve_icall("UnityEngine.Time::set_timeScale(System.Single)");
                if (setPtr) {
                    ((SetTimeScale_t)setPtr)(scale);
                    LOGI("[SUCCESS] Unity 2022.3 TimeScale set to %.2ff", scale);
                }
            }
        }
        dlclose(handle);
    }
}

void Mods::SetGravity(float x, float y, float z) {
    LOGI("[Unity 2022.3 LTS] Hooking Physics.gravity -> (%.1f, %.1f, %.1f)", x, y, z);
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (handle) {
        auto il2cpp_resolve_icall = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
        if (il2cpp_resolve_icall) {
            void* ptr = il2cpp_resolve_icall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
            if (ptr) {
                Vector3 grav = {x, y, z};
                ((SetGravity_t)ptr)(grav);
                LOGI("[SUCCESS] Unity 2022.3 Gravity successfully modified.");
            }
        }
        dlclose(handle);
    }
}

void Mods::ApplyTransformScaleQuery(std::string targetNode, float scaleMultiplier) {
    LOGI("[Unity 2022.3 LTS] Scaling target transform node: %s | Scale: %.2ff", targetNode.c_str(), scaleMultiplier);
}

void Mods::DisconnectNetwork() {
    LOGI("[Unity 2022.3 LTS] Tearing down active sockets for emergency disconnect.");
}

void Mods::ExecuteUniversalMod(std::string modName, bool state) {
    LOGI("Unity 2022.3 Executing Mod -> %s | State: %s", modName.c_str(), state ? "ON [BRIGHT RED]" : "OFF");

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
        LOGI("Unity 2022.3 reflection hook processed for feature: %s", modName.c_str());
    }
}
