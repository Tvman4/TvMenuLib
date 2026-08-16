#include <jni.h>
#include <android/log.h>
#include "Menu.h"

#define TAG "TvMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

void* hack_thread(void*) {
    LOGI("TvMenu initialized successfully!");
    Menu::InitMenu();
    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, nullptr, hack_thread, nullptr);
    return JNI_VERSION_1_6;
}
