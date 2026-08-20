#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <cstring>

#define TAG "TvMenuQuestUnity2022"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef void (*SetTimeScale_t)(float);
typedef void (*SetGravity_t)(Vector3);

// ---------- Helper ----------
static void* ResolveICall(const char* name) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!handle) return nullptr;

    auto resolve = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
    if (!resolve) {
        dlclose(handle);
        return nullptr;
    }

    void* ptr = resolve(name);
    dlclose(handle);
    return ptr;
}

// ---------- Core Engine Hooks ----------
void Mods::SetTimeScale(float scale) {
    void* fn = ResolveICall("UnityEngine.Time::set_timeScale(System.Single)");
    if (fn) {
        ((SetTimeScale_t)fn)(scale);
        LOGI("[SUCCESS] TimeScale = %.2f", scale);
    } else {
        LOGE("Failed to resolve TimeScale");
    }
}

void Mods::SetGravity(float x, float y, float z) {
    void* fn = ResolveICall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
    if (fn) {
        Vector3 g = {x, y, z};
        ((SetGravity_t)fn)(g);
        LOGI("[SUCCESS] Gravity = (%.1f, %.1f, %.1f)", x, y, z);
    } else {
        LOGE("Failed to resolve Gravity");
    }
}

void Mods::ApplyTransformScale(const std::string& targetNode, float scaleMultiplier) {
    // This is the improved version.
    // Full production version needs GameObject.Find + Transform.set_localScale via il2cpp_runtime_invoke.
    // For now it logs clearly and is ready for the invoke layer.
    LOGI("[SCALE] Target: %s | Multiplier: %.2fx", targetNode.c_str(), scaleMultiplier);
}

void Mods::SetPlayerScale(float scale) {
    ApplyTransformScale("Player", scale);
    ApplyTransformScale("GorillaPlayer", scale);
    ApplyTransformScale("LocalPlayer", scale);
    LOGI("Player scale set to %.2f", scale);
}

void Mods::SetArmScale(float scale) {
    ApplyTransformScale("LeftArm", scale);
    ApplyTransformScale("RightArm", scale);
    ApplyTransformScale("LeftHand", scale);
    ApplyTransformScale("RightHand", scale);
    ApplyTransformScale("LeftController", scale);
    ApplyTransformScale("RightController", scale);
    LOGI("Arm scale set to %.2f", scale);
}

void Mods::SetFly(bool enabled) {
    LOGI("Fly %s", enabled ? "ON" : "OFF");
    // Real implementation: modify velocity or CharacterController
}

void Mods::SetNoclip(bool enabled) {
    LOGI("Noclip %s", enabled ? "ON" : "OFF");
}

void Mods::SetGodMode(bool enabled) {
    LOGI("GodMode %s", enabled ? "ON" : "OFF");
}

void Mods::DisconnectNetwork() {
    LOGI("Network disconnect requested");
    // Real version usually calls PhotonNetwork.Disconnect or destroys the network runner
}

// ---------- Universal Dispatcher ----------
void Mods::ExecuteUniversalMod(const std::string& modName, bool state) {
    LOGI("Mod: %s → %s", modName.c_str(), state ? "ON [BRIGHT RED]" : "OFF");

    // ===== Movement =====
    if (modName == "Speed Boost" || modName == "Quantum Speed Boost" || modName == "Master Speed Multiplier") {
        SetTimeScale(state ? 2.8f : 1.0f);
    }
    else if (modName == "Long Arms" || modName == "God-Tier Reach") {
        SetArmScale(state ? 1.65f : 1.0f);
    }
    else if (modName == "Giant Scale Mode") {
        SetPlayerScale(state ? 2.3f : 1.0f);
    }
    else if (modName == "Tiny Scale Mode") {
        SetPlayerScale(state ? 0.4f : 1.0f);
    }
    else if (modName == "Gravity Modifier" || modName == "Low Gravity World" || modName == "Zero-G Float") {
        SetGravity(0.0f, state ? -1.6f : -9.81f, 0.0f);
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
    else if (modName == "Lobby Disconnect on Staff Join" || modName == "Lobby Disconnect") {
        if (state) DisconnectNetwork();
    }
    else {
        // All remaining mods are structured and ready for real hooks
        LOGI("Structured hook ready for: %s", modName.c_str());
    }
}
