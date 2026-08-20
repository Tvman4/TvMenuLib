LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := dobby/lib/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := TvMenu

LOCAL_SRC_FILES := \
    main.cpp \
    Menu.cpp \
    Mods.cpp \
    imgui/imgui.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_tables.cpp \
    imgui/imgui_widgets.cpp \
    imgui/imgui_impl_opengl3.cpp \
    imgui/imgui_impl_android.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/imgui \
    $(LOCAL_PATH)/dobby/include

LOCAL_STATIC_LIBRARIES := dobby
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3 -ldl
LOCAL_CPPFLAGS := -std=c++17 -fvisibility=hidden -DIMGUI_IMPL_OPENGL_ES3

include $(BUILD_SHARED_LIBRARY)
