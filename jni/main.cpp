#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include "Menu.h"

#define TAG "TvMenuQuestMain"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

void* engine_loop(void*) {
    LOGI("TvMenuQuest Engine Loop started (Unity 2019-2022 LTS compatible)");
    TvMenuQuest::InitMenu();

    while (true) {
        TvMenuQuest::RenderUI();
        usleep(50000); // ~20 FPS
    }
    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, nullptr, engine_loop, nullptr);
    pthread_detach(pt);
    LOGI("TvMenuQuest .so injected successfully via JNI_OnLoad");
    return JNI_VERSION_1_6;
}
