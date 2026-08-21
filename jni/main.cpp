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

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay, EGLSurface);
static eglSwapBuffers_t original_eglSwapBuffers = nullptr;
static bool imgui_ready = false;

void InitImGui() {
    if (imgui_ready) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1920.0f, 1080.0f);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init("#version 300 es");

    imgui_ready = true;
    LOGI("ImGui ready");
}

EGLBoolean hooked_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!imgui_ready) {
        InitImGui();
    }

    // Always force open
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
        LOGE("libEGL.so failed");
        return false;
    }

    void* addr = dlsym(handle, "eglSwapBuffers");
    if (!addr) {
        LOGE("eglSwapBuffers not found");
        return false;
    }

    if (DobbyHook(addr, (void*)hooked_eglSwapBuffers, (void**)&original_eglSwapBuffers) == 0) {
        LOGI("Hook success");
        return true;
    }

    LOGE("Hook failed");
    return false;
}

void* MainThread(void*) {
    sleep(6);
    TvMenuQuest::Init();
    InstallHook();
    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t t;
    pthread_create(&t, nullptr, MainThread, nullptr);
    pthread_detach(t);
    return JNI_VERSION_1_6;
}
