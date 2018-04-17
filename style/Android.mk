LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -D__DEBUG__ -g

LOCAL_C_INCLUDES	:=	\
	$(LOCAL_PATH)/../libGPU/base            \
	$(LOCAL_PATH)/../libGPU/base/android 	\
	$(LOCAL_PATH)/../libGPU/filter 		    \
	$(LOCAL_PATH)/../libGPU/bs              \
	$(LOCAL_PATH)/../src

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_SRC_FILES += $(SRCS)

LOCAL_STATIC_LIBRARIES := \
    libgdl      \
	libfilter	\
	libgpu		\
	libbs

LOCAL_SHARED_LIBRARIES := libnative
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog -lGLESv3 -lEGL -landroid -ljnigraphics
LOCAL_MODULE := style

include $(BUILD_SHARED_LIBRARY)