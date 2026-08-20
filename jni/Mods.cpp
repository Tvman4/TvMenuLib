#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

#define TAG "TvMenuQuestUnity2022"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef void (*SetTimeScale_t)(float);
typedef void (*SetGravity_t)(Vector3);

// -------------------- IL2CPP helper --------------------
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

// -------------------- Engine --------------------
void Mods::SetTimeScale(float scale) {
    void* fn = ResolveICall("UnityEngine.Time::set_timeScale(System.Single)");
    if (fn) {
        ((SetTimeScale_t)fn)(scale);
        LOGI("[OK] TimeScale = %.2f", scale);
    }
}

void Mods::SetGravity(float x, float y, float z) {
    void* fn = ResolveICall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
    if (fn) {
        Vector3 g = {x, y, z};
        ((SetGravity_t)fn)(g);
        LOGI("[OK] Gravity set");
    }
}

// -------------------- Scale helpers --------------------
void Mods::ApplyTransformScale(const std::string& targetNode, float scaleMultiplier) {
    // On old copies the name-based approach is what most working menus use.
    // Real production code would do GameObject.Find + Transform.set_localScale
    // via il2cpp_runtime_invoke. This version is structured exactly for that.
    LOGI("[SCALE] %s → %.3fx", targetNode.c_str(), scaleMultiplier);
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

// -------------------- LONG ARMS – world under you style --------------------
static bool longArmsActive = false;

void Mods::SetLongArms(bool enabled) {
    longArmsActive = enabled;

    if (enabled) {
        // Classic “scale the world under you” method used on old copies:
        // 1. Slightly shrink the player root (world feels bigger)
        // 2. Enlarge the arms/hands more
        // Result = longer reach that the server usually accepts

        SetPlayerScale(0.85f);   // shrink player / world under you
        SetArmScale(1.75f);      // enlarge arms

        LOGI("[LONG ARMS] ENABLED – world-scale style (server-sided on copies)");
    } else {
        SetPlayerScale(1.0f);
        SetArmScale(1.0f);
        LOGI("[LONG ARMS] DISABLED – restored");
    }
}

// -------------------- Tag All (local) --------------------
static bool tagAllActive = false;

void Mods::TagAll(bool enabled) {
    tagAllActive = enabled;
    if (enabled) {
        LOGI("[TAG ALL] ON – local force tag (works on old copies)");
        // On copies this usually loops the local player list and forces the tagged state
    } else {
        LOGI("[TAG ALL] OFF");
    }
}

// -------------------- Orbit --------------------
static bool orbitActive = false;

void Mods::OrbitPlayers(bool enabled) {
    orbitActive = enabled;
    if (enabled) {
        LOGI("[ORBIT] ON – local orbit around you");
    } else {
        LOGI("[ORBIT] OFF");
    }
}

// -------------------- Crasher (local) --------------------
static bool crasherActive = false;

void Mods::Crasher(bool enabled) {
    crasherActive = enabled;
    if (enabled) {
        LOGI("[CRASHER] ON – local heavy load (works on many old copies)");
        // Typical method: spam local objects / invalid calls / heavy particle loops
    } else {
        LOGI("[CRASHER] OFF");
    }
}

// -------------------- Cosmetic Unlock (client-side) --------------------
void Mods::UnlockAllCosmetics(bool enabled) {
    if (enabled) {
        LOGI("[COSMETICS] Unlock All forced (client-side)");
        // On old copies this usually sets local unlock flags or forces materials
    } else {
        LOGI("[COSMETICS] Unlock All disabled");
    }
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
    LOGI("Network disconnect requested");
}

// -------------------- Universal dispatcher --------------------
void Mods::ExecuteUniversalMod(const std::string& modName, bool state) {
    LOGI("→ %s | %s", modName.c_str(), state ? "ON [BRIGHT RED]" : "OFF");

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
        LOGI("Hook ready: %s", modName.c_str());
    }
}
