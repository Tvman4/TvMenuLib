#include <jni.h>
#include <unistd.h>
#include <pthread.h>
#include <BNM/Loading.hpp>
#include "Log.h"
#include "Unity.h"
#include "Input.h"
#include "Menu.h"
#include "Mods.h"

static bool g_loaded = false;

static void OnGameTick() {
    if (!g_loaded) return;
    XR::Tick();
    WristMenu::Tick();
}

static void OnBNMLoaded() {
    LOGI("BNM loaded, Unity 2021.3 path");
    U::Init();
    WristMenu::Init();
    g_loaded = true;
}

static void *FallbackLoop(void *) {
    for (int i = 0; i < 200 && !g_loaded; i++) usleep(100 * 1000);
    while (true) {
        if (g_loaded) OnGameTick();
        usleep(11111);
    }
    return nullptr;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    vm->GetEnv((void **)&env, JNI_VERSION_1_6);
    LOGI("JNI_OnLoad libTvMenu.so");
    if (env) BNM::Loading::TryLoadByJNI(env);
    BNM::Loading::AddOnLoadedEvent(OnBNMLoaded);
    pthread_t th;
    pthread_create(&th, nullptr, FallbackLoop, nullptr);
    pthread_detach(th);
    return JNI_VERSION_1_6;
}

extern "C" void TvMenu_Start() {
    OnBNMLoaded();
}
