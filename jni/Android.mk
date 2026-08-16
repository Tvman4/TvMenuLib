LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := TvMenu
LOCAL_SRC_FILES := main.cpp
LOCAL_LDLIBS    := -llog -landroid -lGLESv2
LOCAL_CPPFLAGS  := -std=c++11 -fvisibility=hidden

include $(BUILD_SHARED_LIBRARY)
