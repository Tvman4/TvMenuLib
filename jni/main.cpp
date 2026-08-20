#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include "Menu.h"

#define TAG "TvMenuQuestMain"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

void* engine_loop(void*) {
    // Wait for Unity to fully load (important to prevent crash)
    sleep(10);

    LOGI("TvMenuQuest starting after delay...");
    TvMenuQuest::InitMenu();

    while (true) {
        TvMenuQuest::UpdateInput();
        TvMenuQuest::RenderUI();
        usleep(20000);
    }
    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, nullptr, engine_loop, nullptr);
    pthread_detach(pt);
    LOGI("TvMenuQuest .so injected (delayed start)");
    return JNI_VERSION_1_6;
}
