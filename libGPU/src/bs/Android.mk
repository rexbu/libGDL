LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

LOCAL_SRC_FILES += \
	bs_log.c	\
	bs_socket.c	\
	bs_sign.c	\
	bs_list.c	\
	bs_pool.c	\
	bs_vector.c	\
	bs_lock.c	\
	bs_conf.c	\
	bs_cqueue.c	\
	bs_url.c	\
	bs_common.c	\
	bs_object.c	\
	bs_string.c	\
	bs_data.c
# bs_time里的signal函数在r10里不支持
#When you use the ndk r10, android-21 is used by default but it's not fully retro-compatible with devices running former Android versions. In your case, signal can't be found on your device (but it would run properly on Lollipop).
#When using the NDK, you should use the platform (APP_PLATFORM:=android-XX) that corresponds to your android:minSdkVersion.
#So here you can set APP_PLATFORM:=android-15 inside Application.mk Makefile, and your lib will use the inline version of signal, so it will not look for its symbol at runtime.	
#bs_timer.c	

#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_MODULE := bs

include $(BUILD_STATIC_LIBRARY)
