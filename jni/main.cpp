#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include "Menu.h"

#define TAG "TvMenuQuestMain"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

void* engine_loop(void*) {
    LOGI("TvMenuQuest Engine Loop Active across Unity 2019-2022 LTS.");
    TvMenuQuest::InitMenu();
    
    while (true) {
        TvMenuQuest::RenderUI();
        usleep(50000); // 20 FPS heartbeat loop
    }
    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, nullptr, engine_loop, nullptr);
    LOGI("Library successfully injected and hooked via JNI_OnLoad.");
    return JNI_VERSION_1_6;
}
