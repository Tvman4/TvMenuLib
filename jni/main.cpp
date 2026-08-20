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
static bool imgui_initialized = false;

void InitImGui() {
    if (imgui_initialized) return;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init("#version 300 es");
    imgui_initialized = true;
    LOGI("ImGui ready");
}

EGLBoolean hooked_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!imgui_initialized) InitImGui();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    TvMenuQuest::Draw();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return original_eglSwapBuffers(dpy, surface);
}

bool InstallHook() {
    void* handle = dlopen("libEGL.so", RTLD_LAZY);
    if (!handle) return false;
    void* addr = dlsym(handle, "eglSwapBuffers");
    if (!addr) return false;

    return DobbyHook(addr, (void*)hooked_eglSwapBuffers, (void**)&original_eglSwapBuffers) == 0;
}

void* MainThread(void*) {
    sleep(5);
    TvMenuQuest::Init();
    if (InstallHook()) LOGI("Hook success - menu should appear");
    else LOGE("Hook failed");
    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t t;
    pthread_create(&t, nullptr, MainThread, nullptr);
    pthread_detach(t);
    return JNI_VERSION_1_6;
}
