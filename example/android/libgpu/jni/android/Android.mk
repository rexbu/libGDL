LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -D__DEBUG__ -g
#APP_PLATFORM := android-23

LOCAL_C_INCLUDES	:=	\
	$(LOCAL_PATH)/../base 			\
	$(LOCAL_PATH)/../base/android 	\
	$(LOCAL_PATH)/../filter 		\
	$(LOCAL_PATH)/../bs

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_SRC_FILES += $(SRCS)

LOCAL_STATIC_LIBRARIES := \
	libfilter	\
	libgpu		\
	libbs	

LOCAL_SHARED_LIBRARIES := libnative
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog -lGLESv2 -lEGL -landroid -ljnigraphics
LOCAL_MODULE := GPU

include $(BUILD_SHARED_LIBRARY)
