#if __cplusplus < 202002L
static_assert(false, "ByNameModding requires C++20 and above!");
#endif

#pragma once

// Unity 2021.3.x (il2cpp 28) — GTAG copies / skids
#define UNITY_VER 213
#define UNITY_PATCH_VER 0

#include <android/log.h>
#include <dobby.h>

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if ((void *)ptr == nullptr) return nullptr;
    DobbyHook((void *)ptr, (void *)newMethod, (dobby_dummy_func_t *)&oldBytes);
    return (void *)ptr;
}

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) {
    if ((void *)ptr == nullptr) return nullptr;
    void *tmp = nullptr;
    DobbyHook((void *)ptr, (void *)newMethod, (dobby_dummy_func_t *)&tmp);
    return (void *)ptr;
}

template<typename PTR_T>
inline void Unhook(PTR_T ptr) {
    if ((void *)ptr != nullptr) DobbyDestroy((void *)ptr);
}

#define BNM_DEPRECATED
#define BNM_CLASSES_MANAGEMENT
#define BNM_COROUTINE
#define BNM_USE_IL2CPP_ALLOCATOR

#ifndef NDEBUG
#define BNM_ALLOW_STR_METHODS
#define BNM_ALLOW_SAFE_IS_ALLOCATED
#define BNM_ALLOW_SELF_CHECKS
#define BNM_CHECK_INSTANCE_TYPE
#define BNM_DEBUG
#define BNM_INFO
#define BNM_ERROR
#define BNM_WARNING
#endif

#define BNM_OBFUSCATE(str) str
#define BNM_OBFUSCATE_TMP(str) str

#define BNM_dlopen dlopen
#define BNM_dlsym dlsym
#define BNM_dlclose dlclose
#define BNM_dladdr dladdr

#define BNM_malloc malloc
#define BNM_free free

#define BNM_TAG "TvMenu"

#ifdef BNM_ALLOW_SELF_CHECKS
#define BNM_CHECK_SELF(returnValue) if (!SelfCheck()) return returnValue
#else
#define BNM_CHECK_SELF(returnValue) ((void)0)
#endif

#ifdef BNM_INFO
#define BNM_LOG_INFO(...) ((void)__android_log_print(4, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_INFO(...) ((void)0)
#endif

#ifdef BNM_DEBUG
#define BNM_LOG_DEBUG(...) ((void)__android_log_print(3, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_DEBUG_IF(condition, ...) if (condition) ((void)__android_log_print(3, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_DEBUG(...) ((void)0)
#define BNM_LOG_DEBUG_IF(...) ((void)0)
#endif

#ifdef BNM_ERROR
#define BNM_LOG_ERR(...) ((void)__android_log_print(6, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_ERR_IF(condition, ...) if (condition) ((void)__android_log_print(6, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_ERR(...) ((void)0)
#define BNM_LOG_ERR_IF(...) ((void)0)
#endif

#ifdef BNM_WARNING
#define BNM_LOG_WARN(...) ((void)__android_log_print(5, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_WARN_IF(condition, ...) if (condition) ((void)__android_log_print(5, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_WARN(...) ((void)0)
#define BNM_LOG_WARN_IF(...) ((void)0)
#endif

#define BNM_VER "2.5.2-TvMenu"
