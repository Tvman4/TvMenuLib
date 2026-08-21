#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <string>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TvMenuMods", __VA_ARGS__)

static void* ResolveICall(const char* name) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!handle) return nullptr;
    auto resolve = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
    if (!resolve) return nullptr;
    return resolve(name);
}

void Mods::SetLongArms(bool enabled) {
    LOGI("Long Arms %s", enabled ? "ON" : "OFF");
    // World-scale style for old copies
    void* setScale = ResolveICall("UnityEngine.Transform::set_localScale(UnityEngine.Vector3)");
    if (setScale) {
        LOGI("Long Arms: Transform scale ready");
    }
}

void Mods::SetSpeed(bool enabled) {
    LOGI("Speed Boost %s", enabled ? "ON" : "OFF");
    void* setTS = ResolveICall("UnityEngine.Time::set_timeScale(System.Single)");
    if (setTS) {
        LOGI("Speed: TimeScale ready");
    }
}

void Mods::SetFly(bool enabled) {
    LOGI("Fly %s", enabled ? "ON" : "OFF");
}

void Mods::SetNoclip(bool enabled) {
    LOGI("Noclip %s", enabled ? "ON" : "OFF");
}

void Mods::SetGravity(bool enabled) {
    LOGI("Gravity %s", enabled ? "ON" : "OFF");
    void* grav = ResolveICall("UnityEngine.Physics::set_gravity(UnityEngine.Vector3)");
    if (grav) {
        LOGI("Gravity icall ready");
    }
}

void Mods::SetSuperJump(bool enabled) {
    LOGI("Super Jump %s", enabled ? "ON" : "OFF");
}

void Mods::SetZeroGravity(bool enabled) {
    LOGI("Zero Gravity %s", enabled ? "ON" : "OFF");
}

void Mods::TagAll(bool enabled) {
    LOGI("Tag All %s", enabled ? "ON" : "OFF");
}

void Mods::Orbit(bool enabled) {
    LOGI("Orbit %s", enabled ? "ON" : "OFF");
}

void Mods::Crasher(bool enabled) {
    LOGI("Crasher %s", enabled ? "ON" : "OFF");
}

void Mods::UnlockCosmetics(bool enabled) {
    LOGI("Unlock Cosmetics %s", enabled ? "ON" : "OFF");
}

void Mods::GodMode(bool enabled) {
    LOGI("God Mode %s", enabled ? "ON" : "OFF");
}

void Mods::Fullbright(bool enabled) {
    LOGI("Fullbright %s", enabled ? "ON" : "OFF");
}

void Mods::ExecuteUniversalMod(const std::string& name, bool state) {
    if (name == "Long Arms") SetLongArms(state);
    else if (name == "Speed Boost" || name == "Master Speed") SetSpeed(state);
    else if (name == "Fly") SetFly(state);
    else if (name == "Noclip") SetNoclip(state);
    else if (name == "Gravity") SetGravity(state);
    else if (name == "Super Jump") SetSuperJump(state);
    else if (name == "Zero Gravity") SetZeroGravity(state);
    else if (name == "Tag All") TagAll(state);
    else if (name == "Orbit") Orbit(state);
    else if (name == "Crasher") Crasher(state);
    else if (name == "Unlock All Cosmetics") UnlockCosmetics(state);
    else if (name == "God Mode") GodMode(state);
    else if (name == "Fullbright") Fullbright(state);
}
