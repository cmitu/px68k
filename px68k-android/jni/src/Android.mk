LOCAL_PATH := $(call my-dir)

include ../../../version.txt
include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
PX68K_PATH := ../../..

NDK_PATH := /Users/hissorii/Development/android-ndk-r9b

LOCAL_C_INCLUDES := $(NDK_PATH)/platforms/android-18/arch-arm/usr/include $(LOCAL_PATH)/$(SDL_PATH)/include $(NDK_PATH)/sources/android/support/include \
	$(PX68K_PATH)/x68k $(PX68K_PATH)/m68000 $(PX68K_PATH)/fmgen $(PX68K_PATH)/SDL2 $(PX68K_PATH)/win32api

CPUSRCS= \
	$(PX68K_PATH)/x68k/d68k.c \
	$(PX68K_PATH)/m68000/c68k.c \
	$(PX68K_PATH)/m68000/m68000.c

X68KSRCS= \
	$(PX68K_PATH)/x68k/adpcm.c \
	$(PX68K_PATH)/x68k/bg.c \
	$(PX68K_PATH)/x68k/crtc.c \
	$(PX68K_PATH)/x68k/dmac.c \
	$(PX68K_PATH)/x68k/fdc.c \
	$(PX68K_PATH)/x68k/fdd.c \
	$(PX68K_PATH)/x68k/disk_d88.c \
	$(PX68K_PATH)/x68k/disk_dim.c \
	$(PX68K_PATH)/x68k/disk_xdf.c \
	$(PX68K_PATH)/x68k/gvram.c \
	$(PX68K_PATH)/x68k/ioc.c \
	$(PX68K_PATH)/x68k/irqh.c \
	$(PX68K_PATH)/x68k/mem_wrap.c \
	$(PX68K_PATH)/x68k/mercury.c \
	$(PX68K_PATH)/x68k/mfp.c \
	$(PX68K_PATH)/x68k/palette.c \
	$(PX68K_PATH)/x68k/midi.c \
	$(PX68K_PATH)/x68k/pia.c \
	$(PX68K_PATH)/x68k/rtc.c \
	$(PX68K_PATH)/x68k/sasi.c \
	$(PX68K_PATH)/x68k/scc.c \
	$(PX68K_PATH)/x68k/scsi.c \
	$(PX68K_PATH)/x68k/sram.c \
	$(PX68K_PATH)/x68k/sysport.c \
	$(PX68K_PATH)/x68k/tvram.c
FMGENSRCS= \
	$(PX68K_PATH)/fmgen/fmgen.cpp \
	$(PX68K_PATH)/fmgen/fmg_wrap.cpp \
	$(PX68K_PATH)/fmgen/file.cpp \
	$(PX68K_PATH)/fmgen/fmtimer.cpp \
	$(PX68K_PATH)/fmgen/opm.cpp \
	$(PX68K_PATH)/fmgen/opna.cpp \
	$(PX68K_PATH)/fmgen/psg.cpp

SDLSRCS= \
	$(PX68K_PATH)/SDL2/joystick.c \
	$(PX68K_PATH)/SDL2/juliet.c \
	$(PX68K_PATH)/SDL2/keyboard.c \
	$(PX68K_PATH)/SDL2/mouse.c \
	$(PX68K_PATH)/SDL2/prop.c \
	$(PX68K_PATH)/SDL2/status.c \
	$(PX68K_PATH)/SDL2/timer.c \
	$(PX68K_PATH)/SDL2/dswin.c \
	$(PX68K_PATH)/SDL2/windraw.c \
	$(PX68K_PATH)/SDL2/winui.c \
	$(PX68K_PATH)/SDL2/about.c \
	$(PX68K_PATH)/SDL2/common.c

SDLCXXSRCS= $(PX68K_PATH)/SDL2/winx68k.cpp

WIN32APISRCS= \
	$(PX68K_PATH)/win32api/dosio.c \
	$(PX68K_PATH)/win32api/fake.c \
	$(PX68K_PATH)/win32api/peace.c

PX68KSRCS = $(CPUSRCS) $(X68KSRCS) $(FMGENSRCS) $(SDLSRCS) $(SDLCXXSRCS) $(WIN32APISRCS)

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c $(PX68KSRCS)

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_CFLAGS += -DUSE_OGLES20 -DPX68K_VERSION=$(PX68K_VERSION)

LOCAL_LDLIBS := -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
