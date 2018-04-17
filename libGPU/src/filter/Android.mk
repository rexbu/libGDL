LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

LOCAL_C_INCLUDES	:=	\
	$(LOCAL_PATH)/../base 			\
	$(LOCAL_PATH)/../base/android 	\
	$(LOCAL_PATH)/../filter 		\
	$(LOCAL_PATH)/../bs

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_SRC_FILES += $(SRCS)
	# GPUBackgroundFilter.cpp		\
	# GPUGaussianBlurFilter.cpp	\
	# GPUMedianFilter.cpp			\
	# GPUSaturationFilter.cpp		\
	# GPULuminanceFilter.cpp		\
	# GPUOutlineFilter.cpp		\
	# GPUSobelEdgeFilter.cpp		
#LOCAL_SRC_FILES := 	$(wildcard ./*.cpp)

LOCAL_MODULE := filter

include $(BUILD_STATIC_LIBRARY)
