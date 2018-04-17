LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a x86))

include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -D__DEBUG__ -g
#APP_PLATFORM := android-23

LOCAL_C_INCLUDES	:=	\
    $(LOCAL_PATH)/                          \
	$(LOCAL_PATH)/../libGPU/src/base 			\
	$(LOCAL_PATH)/../libGPU/src/base/android 	\
	$(LOCAL_PATH)/../libGPU/src/filter 		\
	$(LOCAL_PATH)/../libGPU/src/bs

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_SRC_FILES += $(SRCS)

# 支持neon指令
LOCAL_ARM_NEON := true

LOCAL_MODULE := gdl

include $(BUILD_STATIC_LIBRARY)

endif # TARGET_ARCH_ABI == armeabi-v7a || x86
