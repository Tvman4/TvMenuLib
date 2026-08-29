#pragma once
#include <android/log.h>
#define TVMENU_TAG "TvMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TVMENU_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  TVMENU_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TVMENU_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TVMENU_TAG, __VA_ARGS__)
