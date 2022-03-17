#ifndef winx68k_common_h
#define winx68k_common_h

#ifdef _WIN32
#include "windows.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#ifndef _WIN32
#include "../win32api/windows.h"
#endif

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#define	TRUE		1
#define	FALSE		0
#define	SUCCESS		0
#define	FAILURE		1

#undef FASTCALL
#define FASTCALL

#define STDCALL
#define	LABEL
#define	__stdcall

#ifdef PSP
#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH 320
#endif

typedef uint8_t	UINT8;
typedef uint16_t	UINT16;
typedef uint32_t	UINT32;
typedef int8_t	INT8;
typedef int16_t	INT16;
typedef int32_t	INT32;

typedef union {
	struct {
		uint8_t l;
		uint8_t h;
	} b;
	uint16_t w;
} PAIR;

#ifdef __cplusplus
extern "C" {
#endif

void Error(const char* s);
void p6logd(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif //winx68k_common_h
