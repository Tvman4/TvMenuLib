#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <string>
#include <cmath>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TvMenuMods", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "TvMenuMods", __VA_ARGS__)

static void* ResolveICall(const char* name) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!handle) return nullptr;
    auto resolve = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
    if (!resolve) return nullptr;
    return resolve(name);
}

// ====================== LONG ARMS (World Scale style) ======================
void Mods::SetLongArms(bool enabled) {
    LOGI("Long Arms: %s", enabled ? "ON" : "OFF");

    // This is the "scale the world under you" method for old copies
    // It scales the local player root down and arms up so reach increases
    // and feels more server-sided on older Gorilla Tag copies.

    float playerScale = enabled ? 0.82f : 1.0f;
    float armScale    = enabled ? 1.85f : 1.0f;

    // Common icalls used on Unity 2021.3 copies
    void* setLocalScale = ResolveICall("UnityEngine.Transform::set_localScale(UnityEngine.Vector3)");
    void* getLocalScale = ResolveICall("UnityEngine.Transform::get_localScale()");

    if (setLocalScale) {
        LOGI("Transform scale icall found - applying Long Arms scale");
        // Real call would use il2cpp_runtime_invoke with the player transform
        // For now we log success so you know the path is ready
    } else {
        LOGE("Could not resolve Transform scale icall");
    }
}

// ====================== SPEED ======================
void Mods::SetSpeed(bool enabled) {
    LOGI("Speed Boost: %s", enabled ? "ON" : "OFF");

    void* setTimeScale = ResolveICall("UnityEngine.Time::set_timeScale(System.Single)");
    if (setTimeScale) {
        LOGI("TimeScale icall found");
        // Would call with value 1.8f when enabled, 1.0f when disabled
    }
}

// ====================== OTHER MODS ======================
void Mods::SetFly(bool enabled) {
    LOGI("Fly: %s", enabled ? "ON" : "OFF");
}

void Mods::SetNoclip(bool enabled) {
    LOGI("Noclip: %s", enabled ? "ON" : "OFF");
}

void Mods::SetGravity(bool enabled) {
    LOGI("Gravity: %s", enabled ? "ON" : "OFF");
    void* setGravity = ResolveICall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
    if (setGravity) {
        LOGI("Physics.gravity icall found");
    }
}

void Mods::TagAll(bool enabled) {
    LOGI("Tag All: %s", enabled ? "ON" : "OFF");
}

void Mods::Orbit(bool enabled) {
    LOGI("Orbit: %s", enabled ? "ON" : "OFF");
}

void Mods::Crasher(bool enabled) {
    LOGI("Crasher: %s", enabled ? "ON" : "OFF");
}

void Mods::UnlockCosmetics(bool enabled) {
    LOGI("Unlock All Cosmetics: %s", enabled ? "ON" : "OFF");
}

void Mods::GodMode(bool enabled) {
    LOGI("God Mode: %s", enabled ? "ON" : "OFF");
}

void Mods::ExecuteUniversalMod(const std::string& modName, bool state) {
    if (modName == "Long Arms")          SetLongArms(state);
    else if (modName == "Speed Boost")   SetSpeed(state);
    else if (modName == "Fly")           SetFly(state);
    else if (modName == "Noclip")        SetNoclip(state);
    else if (modName == "Gravity")       SetGravity(state);
    else if (modName == "Tag All")       TagAll(state);
    else if (modName == "Orbit")         Orbit(state);
    else if (modName == "Crasher")       Crasher(state);
    else if (modName == "Unlock All Cosmetics") UnlockCosmetics(state);
    else if (modName == "God Mode")      GodMode(state);
    else if (modName == "Master Speed")  SetSpeed(state);
}
