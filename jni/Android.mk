LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := ../third_party/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := TvMenu
LOCAL_CPPFLAGS += -std=c++20 -fvisibility=hidden -O2
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../src \
    $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../third_party/BNM-Android/include \
    $(LOCAL_PATH)/../third_party/BNM-Android/external/include \
    $(LOCAL_PATH)/../third_party/Dobby/include

LOCAL_SRC_FILES := \
    ../src/main.cpp \
    ../src/Unity.cpp \
    ../src/Input.cpp \
    ../src/Menu.cpp \
    ../src/Mods.cpp

LOCAL_STATIC_LIBRARIES := dobby BNM
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3
include $(BUILD_SHARED_LIBRARY)

BNM_DIR := $(LOCAL_PATH)/../third_party/BNM-Android
include $(BNM_DIR)/Android.mk
