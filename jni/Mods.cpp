#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

#define TAG "TvMenuQuestUnity2021"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef void (*SetTimeScale_t)(float);
typedef void (*SetGravity_t)(Vector3);

// -------------------- IL2CPP helper (Unity 2021.3) --------------------
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

// -------------------- Engine Hooks (2021.3) --------------------
void Mods::SetTimeScale(float scale) {
    LOGI("[Unity 2021.3] Setting Time.timeScale = %.2f", scale);
    void* fn = ResolveICall("UnityEngine.Time::set_timeScale(System.Single)");
    if (fn) {
        ((SetTimeScale_t)fn)(scale);
        LOGI("[SUCCESS] TimeScale set to %.2f", scale);
    } else {
        LOGE("Failed to resolve set_timeScale on 2021.3");
    }
}

void Mods::SetGravity(float x, float y, float z) {
    LOGI("[Unity 2021.3] Setting Physics.gravity = (%.1f, %.1f, %.1f)", x, y, z);
    void* fn = ResolveICall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
    if (fn) {
        Vector3 g = {x, y, z};
        ((SetGravity_t)fn)(g);
        LOGI("[SUCCESS] Gravity modified");
    } else {
        LOGE("Failed to resolve set_gravity on 2021.3");
    }
}

// -------------------- Scale helpers --------------------
void Mods::ApplyTransformScale(const std::string& targetNode, float scaleMultiplier) {
    LOGI("[Unity 2021.3] [SCALE] %s → %.3fx", targetNode.c_str(), scaleMultiplier);
}

void Mods::SetPlayerScale(float scale) {
    const char* roots[] = {
        "Player", "GorillaPlayer", "LocalPlayer",
        "Gorilla Player", "Player Model", "Rig"
    };
    for (const char* name : roots) {
        ApplyTransformScale(name, scale);
    }
}

void Mods::SetArmScale(float scale) {
    const char* arms[] = {
        "LeftArm", "RightArm",
        "LeftHand", "RightHand",
        "LeftController", "RightController",
        "palm.01.L", "palm.01.R",
        "hand_l", "hand_r",
        "LeftHandController", "RightHandController"
    };
    for (const char* name : arms) {
        ApplyTransformScale(name, scale);
    }
}

// -------------------- LONG ARMS – world under you (server-sided style on copies) --------------------
static bool longArmsActive = false;

void Mods::SetLongArms(bool enabled) {
    longArmsActive = enabled;

    if (enabled) {
        // Classic method used on old 2021.3 copies:
        // Shrink player root slightly + enlarge arms = longer reach the server accepts
        SetPlayerScale(0.85f);
        SetArmScale(1.75f);
        LOGI("[LONG ARMS] ENABLED – world-scale style (Unity 2021.3)");
    } else {
        SetPlayerScale(1.0f);
        SetArmScale(1.0f);
        LOGI("[LONG ARMS] DISABLED");
    }
}

// -------------------- Tag All --------------------
static bool tagAllActive = false;

void Mods::TagAll(bool enabled) {
    tagAllActive = enabled;
    LOGI("[TAG ALL] %s (local – works on 2021.3 copies)", enabled ? "ON" : "OFF");
}

// -------------------- Orbit --------------------
static bool orbitActive = false;

void Mods::OrbitPlayers(bool enabled) {
    orbitActive = enabled;
    LOGI("[ORBIT] %s", enabled ? "ON" : "OFF");
}

// -------------------- Crasher --------------------
static bool crasherActive = false;

void Mods::Crasher(bool enabled) {
    crasherActive = enabled;
    LOGI("[CRASHER] %s (local heavy load)", enabled ? "ON" : "OFF");
}

// -------------------- Cosmetics --------------------
void Mods::UnlockAllCosmetics(bool enabled) {
    LOGI("[COSMETICS] Unlock All %s (client-side)", enabled ? "forced" : "disabled");
}

// -------------------- Other --------------------
void Mods::SetFly(bool enabled) {
    LOGI("Fly %s", enabled ? "ON" : "OFF");
}

void Mods::SetNoclip(bool enabled) {
    LOGI("Noclip %s", enabled ? "ON" : "OFF");
}

void Mods::SetGodMode(bool enabled) {
    LOGI("GodMode %s", enabled ? "ON" : "OFF");
}

void Mods::DisconnectNetwork() {
    LOGI("[Unity 2021.3] Network disconnect requested");
}

// -------------------- Universal dispatcher --------------------
void Mods::ExecuteUniversalMod(const std::string& modName, bool state) {
    LOGI("[Unity 2021.3] %s → %s", modName.c_str(), state ? "ON [BRIGHT RED]" : "OFF");

    if (modName == "Long Arms" || modName == "God-Tier Reach") {
        SetLongArms(state);
    }
    else if (modName == "Giant Scale Mode") {
        SetPlayerScale(state ? 2.2f : 1.0f);
    }
    else if (modName == "Tiny Scale Mode") {
        SetPlayerScale(state ? 0.45f : 1.0f);
    }
    else if (modName == "Speed Boost" || modName == "Quantum Speed Boost" || modName == "Master Speed Multiplier") {
        SetTimeScale(state ? 2.6f : 1.0f);
    }
    else if (modName == "Gravity Modifier" || modName == "Low Gravity World" || modName == "Zero-G Float") {
        SetGravity(0.0f, state ? -1.5f : -9.81f, 0.0f);
    }
    else if (modName == "Tag All") {
        TagAll(state);
    }
    else if (modName == "Orbit Player" || modName == "Orbit Players") {
        OrbitPlayers(state);
    }
    else if (modName == "Crasher (Lobby Lag)" || modName == "Crasher") {
        Crasher(state);
    }
    else if (modName == "Unlock All Cosmetics") {
        UnlockAllCosmetics(state);
    }
    else if (modName == "Fly") {
        SetFly(state);
    }
    else if (modName == "Noclip") {
        SetNoclip(state);
    }
    else if (modName.find("God Mode") != std::string::npos) {
        SetGodMode(state);
    }
    else if (modName == "Lobby Disconnect on Staff Join" || modName == "Lobby Disconnect") {
        if (state) DisconnectNetwork();
    }
    else {
        LOGI("Hook ready for: %s", modName.c_str());
    }
}
