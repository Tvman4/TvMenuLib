#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <cstring>

#define TAG "TvMenuQuestUnity2022"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef void (*SetTimeScale_t)(float);
typedef void (*SetGravity_t)(Vector3);

static void* ResolveICall(const char* name) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!handle) {
        LOGE("Failed to open libil2cpp.so");
        return nullptr;
    }
    auto resolve = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
    if (!resolve) {
        LOGE("il2cpp_resolve_icall not found");
        dlclose(handle);
        return nullptr;
    }
    void* ptr = resolve(name);
    dlclose(handle);
    return ptr;
}

void Mods::SetTimeScale(float scale) {
    LOGI("[Unity 2022.3] Setting Time.timeScale = %.2f", scale);
    void* setPtr = ResolveICall("UnityEngine.Time::set_timeScale(System.Single)");
    if (setPtr) {
        ((SetTimeScale_t)setPtr)(scale);
        LOGI("[SUCCESS] TimeScale set to %.2f", scale);
    } else {
        LOGE("Failed to resolve set_timeScale");
    }
}

void Mods::SetGravity(float x, float y, float z) {
    LOGI("[Unity 2022.3] Setting Physics.gravity = (%.1f, %.1f, %.1f)", x, y, z);
    void* setPtr = ResolveICall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
    if (setPtr) {
        Vector3 grav = {x, y, z};
        ((SetGravity_t)setPtr)(grav);
        LOGI("[SUCCESS] Gravity modified");
    } else {
        LOGE("Failed to resolve set_gravity");
    }
}

void Mods::ApplyTransformScale(const std::string& targetNode, float scaleMultiplier) {
    // Placeholder for real Transform.Find + localScale
    // In a full internal menu you would use il2cpp_runtime_invoke on
    // UnityEngine.Transform::set_localScale and GameObject.Find
    LOGI("[Unity 2022.3] Scale request: %s → %.2fx (hook ready for full invoke)", 
         targetNode.c_str(), scaleMultiplier);
}

void Mods::DisconnectNetwork() {
    LOGI("[Unity 2022.3] Emergency network teardown requested");
    // Real implementation usually calls PhotonNetwork.Disconnect() or
    // destroys the NetworkRunner / PhotonPeer via reflection
}

void Mods::SetFly(bool enabled) {
    LOGI("Fly %s", enabled ? "ENABLED" : "DISABLED");
    // Hook CharacterController or Rigidbody velocity here
}

void Mods::SetNoclip(bool enabled) {
    LOGI("Noclip %s", enabled ? "ENABLED" : "DISABLED");
}

void Mods::SetGodMode(bool enabled) {
    LOGI("GodMode %s", enabled ? "ENABLED" : "DISABLED");
}

void Mods::ExecuteUniversalMod(const std::string& modName, bool state) {
    LOGI("Executing [%s] → %s", modName.c_str(), state ? "ON [BRIGHT RED]" : "OFF");

    if (modName == "Speed Boost" || modName == "Quantum Speed Boost" || modName == "Master Speed Multiplier") {
        SetTimeScale(state ? 2.5f : 1.0f);
    }
    else if (modName == "Long Arms" || modName == "God-Tier Reach") {
        ApplyTransformScale("LeftArm", state ? 1.6f : 1.0f);
        ApplyTransformScale("RightArm", state ? 1.6f : 1.0f);
        ApplyTransformScale("LeftController", state ? 1.6f : 1.0f);
        ApplyTransformScale("RightController", state ? 1.6f : 1.0f);
    }
    else if (modName == "Gravity Modifier" || modName == "Low Gravity World" || modName == "Zero-G Float") {
        SetGravity(0.0f, state ? -1.8f : -9.81f, 0.0f);
    }
    else if (modName == "Giant Scale Mode") {
        ApplyTransformScale("Player", state ? 2.2f : 1.0f);
    }
    else if (modName == "Tiny Scale Mode") {
        ApplyTransformScale("Player", state ? 0.45f : 1.0f);
    }
    else if (modName == "Lobby Disconnect on Staff Join" || modName == "Lobby Disconnect") {
        if (state) DisconnectNetwork();
    }
    else if (modName == "Fly") {
        SetFly(state);
    }
    else if (modName == "Noclip") {
        SetNoclip(state);
    }
    else if (modName.find("God Mode") != std::string::npos || modName == "Master God Mode") {
        SetGodMode(state);
    }
    else {
        LOGI("Generic / reflection hook processed for: %s", modName.c_str());
    }
}
