# system platform
ifeq "$(PLATFORM)" ""
PLATFORM := $(shell uname)
endif

ifdef SDL
ifeq "$(PLATFORM)" "Darwin"
SDL_CONFIG?= ./osx/sdl-config-mac
else
SDL_CONFIG?= sdl-config
endif
else
ifeq "$(PLATFORM)" "Darwin"
SDL2 :=1
SDL_CONFIG?= ./osx/sdl2-config-mac
else
SDL2 :=1
SDL_CONFIG?= sdl2-config
endif
endif

ifeq ($(shell uname -m),aarch64)
MOPT=
else
ifeq ($(shell uname -m),x86_64)
MOPT=
else
ifeq ($(shell uname -m),armv8l)
MOPT=
else
MOPT=
endif
endif
endif

ifdef SDL
PROGRAM = px68k.sdl
CDEBUGFLAGS = -DSDL1
else
PROGRAM = px68k.sdl2
CDEBUGFLAGS = -DSDL2
endif

include version.txt

CC	 = gcc -std=c11
CXX	 = c++ -std=c++11
CXXLINK	 = $(CXX)
RM	 = rm -f
TAGS	 = etags
DEPEND	 = gccmakedep
DEPEND_DEFINES =

# for debug
CDEBUGFLAGS += -g -O0 -fno-strict-aliasing
# CDEBUGFLAGS += -Os  -fstrict-aliasing

#
# disable sound
#
# CDEBUGFLAGS+= -DNO_SOUND

#
# Big endian (PowerPC...) not support
#
# CDEBUGFLAGS+= -D__BIG_ENDIAN__

#
# disable mercury unit
#
# CDEBUGFLAGS+= -DNO_MERCURY

#
# enable RFMDRV
#
# CDEBUGFLAGS+= -DRFMDRV

#
# for Opt.
#
# CDEBUGFLAGS= -O3
# CDEBUGFLAGS+= -funroll-loops
# CDEBUGFLAGS+= -fomit-frame-pointer
# CDEBUGFLAGS+= -ffast-math

# CDEBUGFLAGS+= -march=pentium-m
# CDEBUGFLAGS+= -msse -mfpmath=sse

#
# for DEBUG
#
# CDEBUGFLAGS= -O0
# CDEBUGFLAGS+= -g
# CDEBUGFLAGS+= -W -Wall -Wuninitialized
# CDEBUGFLAGS+= -Wunused
# CDEBUGFLAGS+= -Werror
# CDEBUGFLAGS+= -DINLINE=
# CDEBUGFLAGS+= -DUSE_GAS

CDEBUGFLAGS+=-DPX68K_VERSION=$(PX68K_VERSION)

CDEBUGFLAGS+=-DHAVE_STDINT_H

SDL_INCLUDE=	`$(SDL_CONFIG) --cflags`
SDL_LIB=		`$(SDL_CONFIG) --libs`

LDLIBS = -lm

ifeq "$(PLATFORM)" "Darwin"
LDLIBS+=  -framework Cocoa -framework CoreMIDI
else
ifeq "$(PLATFORM)" "Linux"
# 
else
# Cygwin for MIDI (winmm.lib)
LDLIBS+= -lwinmm
endif
endif

EXTRA_INCLUDES= -I./SDL2 -I./x68k -I./fmgen -I./win32api $(SDL_INCLUDE)

CXXDEBUGFLAGS= $(CDEBUGFLAGS)

CFLAGS= $(MOPT) $(CDEBUGFLAGS) $(EXTRA_INCLUDES)
CXXFLAGS= $(MOPT) $(CXXDEBUGFLAGS) $(EXTRA_INCLUDES)
CXXLDOPTIONS= $(CXXDEBUGFLAGS)

# CFLAGS   += -DC68K_NO_JUMP_TABLE
# CFLAGS   += -DC68K_CONST_JUMP_TABLE

CPUOBJS= x68k/d68k.o m68000/m68000.o
C68KOBJS= m68000/c68k/c68k.o m68000/c68k/c68kexec.o

X68KOBJS= x68k/adpcm.o x68k/bg.o x68k/crtc.o x68k/dmac.o x68k/fdc.o x68k/fdd.o x68k/disk_d88.o x68k/disk_dim.o x68k/disk_xdf.o x68k/gvram.o x68k/ioc.o x68k/irqh.o x68k/mem_wrap.o x68k/mercury.o x68k/mfp.o x68k/palette.o x68k/midi.o x68k/pia.o x68k/rtc.o x68k/sasi.o x68k/scc.o x68k/scsi.o x68k/sram.o x68k/sysport.o x68k/tvram.o

FMGENOBJS= fmgen/fmgen.o fmgen/fmg_wrap.o fmgen/file.o fmgen/fmtimer.o fmgen/opm.o fmgen/opna.o fmgen/psg.o

SDL2OBJS= SDL2/juliet.o SDL2/mouse.o SDL2/status.o SDL2/timer.o SDL2/about.o SDL2/common.o SDL2/prop.o SDL2/joystick.o SDL2/winui.o SDL2/dswin.o SDL2/keyboard.o 

ifdef SDL
SDLOBJS= SDL2/SDL1/windraw.o
SDLCXXOBJS= SDL2/SDL1/winx68k.o
else
SDLOBJS= SDL2/windraw.o
SDLCXXOBJS= SDL2/winx68k.o
endif


WIN32APIOBJS= win32api/dosio.o win32api/fake.o win32api/peace.o

COBJS=		$(X68KOBJS) $(SDL2OBJS) $(SDLOBJS) $(WIN32APIOBJS) $(CPUOBJS) $(C68KOBJS)
CXXOBJS=	$(FMGENOBJS) $(SDLCXXOBJS)
OBJS=		$(COBJS) $(CXXOBJS)

CSRCS=		$(COBJS:.o=.c)
CXXSRCS=	$(CXXOBJS:.o=.cpp)
SRCS=		$(CSRCS) $(CXXSRCS)

.SUFFIXES: .c .cpp

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $*.c

.cpp.o:
	$(CXX) -o $@ $(CXXFLAGS) -c $*.cpp

all:: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(RM) $@
	$(CXXLINK) $(MOPT) -o $(PROGRAM) $(CXXLDOPTIONS) $(OBJS) $(SDL_LIB) $(LDLIBS)

depend::
	$(DEPEND) -- $(CXXFLAGS) $(DEPEND_DEFINES) -- $(SRCS)

clean::
	$(RM) $(PROGRAM)
	$(RM) $(OBJS)
	$(RM) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut   "#"*

tags::
	find . -name "*.h" -o -name "*.c" -o -name "*.cpp" | $(TAGS) -

mac::
	-rm -rf "$(PROGRAM).app/"
	mkdir "$(PROGRAM).app/"
	cp -r "osx/Contents/" "$(PROGRAM).app/Contents"
	cp $(PROGRAM) "$(PROGRAM).app/Contents/MacOS/px68k"

