#include "Mods.h"
#include <android/log.h>
#include <dlfcn.h>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TvMenuMods", __VA_ARGS__)

static std::atomic<bool> longArmsOn{false};
static std::atomic<bool> speedOn{false};
static std::atomic<bool> flyOn{false};
static std::atomic<bool> noclipOn{false};
static std::atomic<bool> gravityOn{false};
static std::atomic<bool> superJumpOn{false};
static std::atomic<bool> airJumpOn{false};
static std::atomic<bool> tagAllOn{false};
static std::atomic<bool> orbitOn{false};
static std::atomic<bool> ghostOn{false};
static std::atomic<bool> invisOn{false};

static void* ResolveICall(const char* name) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!handle) return nullptr;
    auto resolve = (void* (*)(const char*))dlsym(handle, "il2cpp_resolve_icall");
    if (!resolve) return nullptr;
    return resolve(name);
}

void Mods::SetLongArms(bool enabled) {
    longArmsOn = enabled;
    LOGI("Long Arms %s", enabled ? "ON" : "OFF");
}

void Mods::SetSpeed(bool enabled) {
    speedOn = enabled;
    LOGI("Speed %s", enabled ? "ON" : "OFF");
    if (enabled) {
        std::thread([] { while (speedOn) std::this_thread::sleep_for(std::chrono::milliseconds(16)); }).detach();
    }
}

void Mods::SetFly(bool enabled) {
    flyOn = enabled;
    LOGI("Fly %s", enabled ? "ON" : "OFF");
    if (enabled) {
        std::thread([] { while (flyOn) std::this_thread::sleep_for(std::chrono::milliseconds(16)); }).detach();
    }
}

void Mods::SetNoclip(bool enabled) {
    noclipOn = enabled;
    LOGI("Noclip %s", enabled ? "ON" : "OFF");
}

void Mods::SetGravity(bool enabled) {
    gravityOn = enabled;
    LOGI("Gravity %s", enabled ? "ON" : "OFF");
}

void Mods::SetSuperJump(bool enabled) {
    superJumpOn = enabled;
    LOGI("Super Jump %s", enabled ? "ON" : "OFF");
}

void Mods::SetZeroGravity(bool enabled) {
    SetGravity(enabled);
}

void Mods::SetAirJump(bool enabled) {
    airJumpOn = enabled;
    LOGI("Air Jump %s", enabled ? "ON" : "OFF");
}

void Mods::TagAll(bool enabled) {
    tagAllOn = enabled;
    LOGI("Tag All %s", enabled ? "ON" : "OFF");
}

void Mods::Orbit(bool enabled) {
    orbitOn = enabled;
    LOGI("Orbit %s", enabled ? "ON" : "OFF");
}

void Mods::GhostMonkey(bool enabled) {
    ghostOn = enabled;
    LOGI("Ghost Monkey %s", enabled ? "ON" : "OFF");
}

void Mods::InvisAll(bool enabled) {
    invisOn = enabled;
    LOGI("Invis All %s", enabled ? "ON" : "OFF");
}

void Mods::UnlockCosmetics(bool enabled) {
    LOGI("Unlock Cosmetics %s (Client)", enabled ? "ON" : "OFF");
}

void Mods::RainbowMonkey(bool enabled) {
    LOGI("Rainbow Monkey %s (Client)", enabled ? "ON" : "OFF");
}

void Mods::Fullbright(bool enabled) {
    LOGI("Fullbright %s", enabled ? "ON" : "OFF");
}

void Mods::ESP(bool enabled) {
    LOGI("ESP %s", enabled ? "ON" : "OFF");
}

void Mods::Tracers(bool enabled) {
    LOGI("Tracers %s", enabled ? "ON" : "OFF");
}

void Mods::Disconnect(bool enabled) {
    LOGI("Disconnect called");
}

void Mods::ExecuteUniversalMod(const std::string& name, bool state) {
    if (name == "Long Arms") SetLongArms(state);
    else if (name == "Speed Boost" || name == "Master Speed") SetSpeed(state);
    else if (name == "Fly") SetFly(state);
    else if (name == "Noclip") SetNoclip(state);
    else if (name == "Gravity" || name == "Zero Gravity") SetGravity(state);
    else if (name == "Super Jump") SetSuperJump(state);
    else if (name == "Air Jump") SetAirJump(state);
    else if (name == "Tag All") TagAll(state);
    else if (name == "Orbit") Orbit(state);
    else if (name == "Ghost Monkey") GhostMonkey(state);
    else if (name == "Invis All") InvisAll(state);
    else if (name == "Unlock All Cosmetics") UnlockCosmetics(state);
    else if (name == "Rainbow Monkey") RainbowMonkey(state);
    else if (name == "Fullbright") Fullbright(state);
    else if (name == "ESP") ESP(state);
    else if (name == "Tracers") Tracers(state);
    else if (name == "Disconnect") Disconnect(state);
}
