LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_FILENAME = libnative
LOCAL_CFLAGS	:= -D__ANDROID__ -D__DEBUG__ -g

LOCAL_SRC_FILES := 	\
	NativeLoad.cpp	

LOCAL_LDLIBS    := -llog -landroid -ldl
LOCAL_MODULE    := native

include $(BUILD_SHARED_LIBRARY)

