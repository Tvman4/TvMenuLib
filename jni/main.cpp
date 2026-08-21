#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "dobby.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "Menu.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TvMenuMain", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "TvMenuMain", __VA_ARGS__)

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surface);
static eglSwapBuffers_t original_eglSwapBuffers = nullptr;
static bool imgui_initialized = false;

void InitImGui() {
    if (imgui_initialized) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1920, 1080);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init("#version 300 es");

    imgui_initialized = true;
    LOGI("ImGui initialized successfully");
}

EGLBoolean hooked_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!imgui_initialized) {
        InitImGui();
    }

    // Force menu open for testing
    TvMenuQuest::isOpen = true;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    TvMenuQuest::Draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return original_eglSwapBuffers(dpy, surface);
}

bool InstallHook() {
    void* handle = dlopen("libEGL.so", RTLD_NOW);
    if (!handle) {
        LOGE("Failed to open libEGL.so");
        return false;
    }

    void* addr = dlsym(handle, "eglSwapBuffers");
    if (!addr) {
        LOGE("eglSwapBuffers symbol not found");
        return false;
    }

    LOGI("eglSwapBuffers found at %p", addr);

    int result = DobbyHook(addr, (void*)hooked_eglSwapBuffers, (void**)&original_eglSwapBuffers);
    if (result == 0) {
        LOGI("DobbyHook SUCCESS");
        return true;
    }

    LOGE("DobbyHook FAILED with code %d", result);
    return false;
}

void* MainThread(void*) {
    sleep(8);

    LOGI("MainThread started");
    TvMenuQuest::Init();

    if (InstallHook()) {
        LOGI("Hook installed - menu should now appear");
    } else {
        LOGE("Hook failed - menu will not appear");
    }

    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("JNI_OnLoad called");
    pthread_t t;
    pthread_create(&t, nullptr, MainThread, nullptr);
    pthread_detach(t);
    return JNI_VERSION_1_6;
}
