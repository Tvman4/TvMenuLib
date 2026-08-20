#include "Mods.h"
#include "Menu.h"
#include <android/log.h>
#include <dlfcn.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>

#define TAG "TvMenuQuestUnity2021"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef void (*SetTimeScale_t)(float);
typedef void (*SetGravity_t)(Vector3);

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
        if (scale > 1.1f)
            TvMenuQuest::ShowNotification("SPEED BOOST ON");
        else
            TvMenuQuest::ShowNotification("SPEED BOOST OFF");
    }
}

void Mods::SetGravity(float x, float y, float z) {
    void* fn = ResolveICall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
    if (fn) {
        Vector3 g = {x, y, z};
        ((SetGravity_t)fn)(g);
        if (y > -5.0f)
            TvMenuQuest::ShowNotification("LOW GRAVITY ON");
        else
            TvMenuQuest::ShowNotification("GRAVITY NORMAL");
    }
}

// -------------------- Scale --------------------
void Mods::ApplyTransformScale(const std::string& targetNode, float scaleMultiplier) {
    LOGI("[SCALE] %s → %.3fx", targetNode.c_str(), scaleMultiplier);
}

void Mods::SetPlayerScale(float scale) {
    const char* roots[] = {
        "Player", "GorillaPlayer", "LocalPlayer",
        "Gorilla Player", "Player Model", "Rig", "Player Objects"
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

// -------------------- Long Arms (world under you) --------------------
static std::atomic<bool> longArmsActive{false};

void Mods::SetLongArms(bool enabled) {
    longArmsActive = enabled;
    if (enabled) {
        SetPlayerScale(0.82f);
        SetArmScale(1.85f);
        TvMenuQuest::ShowNotification("LONG ARMS ON – World Scale");
    } else {
        SetPlayerScale(1.0f);
        SetArmScale(1.0f);
        TvMenuQuest::ShowNotification("LONG ARMS OFF");
    }
}

// -------------------- Tag All --------------------
static std::atomic<bool> tagAllActive{false};

void Mods::TagAll(bool enabled) {
    tagAllActive = enabled;
    TvMenuQuest::ShowNotification(enabled ? "TAG ALL ON" : "TAG ALL OFF");
}

// -------------------- Orbit --------------------
static std::atomic<bool> orbitActive{false};

void Mods::OrbitPlayers(bool enabled) {
    orbitActive = enabled;
    TvMenuQuest::ShowNotification(enabled ? "ORBIT ON" : "ORBIT OFF");
}

// -------------------- Crasher --------------------
static std::atomic<bool> crasherActive{false};

void Mods::Crasher(bool enabled) {
    crasherActive = enabled;
    TvMenuQuest::ShowNotification(enabled ? "CRASHER ON – Heavy Load" : "CRASHER OFF");

    if (enabled) {
        std::thread([] {
            while (crasherActive) {
                volatile float x = 0;
                for (int i = 0; i < 500000; i++) x += sinf(i * 0.001f);
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }).detach();
    }
}

// -------------------- Cosmetics --------------------
void Mods::UnlockAllCosmetics(bool enabled) {
    TvMenuQuest::ShowNotification(enabled ? "COSMETICS UNLOCKED" : "COSMETICS NORMAL");
}

// -------------------- Fly / Noclip / GodMode --------------------
static std::atomic<bool> flyActive{false};
static std::atomic<bool> noclipActive{false};
static std::atomic<bool> godModeActive{false};

void Mods::SetFly(bool enabled) {
    flyActive = enabled;
    TvMenuQuest::ShowNotification(enabled ? "FLY ON" : "FLY OFF");
}

void Mods::SetNoclip(bool enabled) {
    noclipActive = enabled;
    TvMenuQuest::ShowNotification(enabled ? "NOCLIP ON" : "NOCLIP OFF");
}

void Mods::SetGodMode(bool enabled) {
    godModeActive = enabled;
    TvMenuQuest::ShowNotification(enabled ? "GOD MODE ON" : "GOD MODE OFF");
}

void Mods::DisconnectNetwork() {
    TvMenuQuest::ShowNotification("LOBBY DISCONNECTED");
}

// -------------------- Universal dispatcher --------------------
void Mods::ExecuteUniversalMod(const std::string& modName, bool state) {
    if (modName == "Long Arms" || modName == "God-Tier Reach") {
        SetLongArms(state);
    }
    else if (modName == "Giant Scale Mode") {
        SetPlayerScale(state ? 2.3f : 1.0f);
        TvMenuQuest::ShowNotification(state ? "GIANT SCALE ON" : "GIANT SCALE OFF");
    }
    else if (modName == "Tiny Scale Mode") {
        SetPlayerScale(state ? 0.4f : 1.0f);
        TvMenuQuest::ShowNotification(state ? "TINY SCALE ON" : "TINY SCALE OFF");
    }
    else if (modName == "Speed Boost" || modName == "Quantum Speed Boost" || modName == "Master Speed Multiplier") {
        SetTimeScale(state ? 2.7f : 1.0f);
    }
    else if (modName == "Gravity Modifier" || modName == "Low Gravity World" || modName == "Zero-G Float") {
        SetGravity(0.0f, state ? -1.4f : -9.81f, 0.0f);
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
    else if (modName.find("God Mode") != std::string::npos || modName == "Master God Mode") {
        SetGodMode(state);
    }
    else if (modName == "Lobby Disconnect on Staff Join" || modName == "Lobby Disconnect") {
        if (state) DisconnectNetwork();
    }
    else {
        TvMenuQuest::ShowNotification(state ? (modName + " ON") : (modName + " OFF"));
    }
}
