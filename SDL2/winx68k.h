#ifndef winx68k_wincore_h
#define winx68k_wincore_h

#include "common.h"

#ifdef RFMDRV
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern int32_t rfd_sock;
#endif

#define vline HOGEvline // workaround for redefinition of 'vline'

#define		SCREEN_WIDTH		768
#define		SCREEN_HEIGHT		512

#define		FULLSCREEN_WIDTH	800
#define		FULLSCREEN_HEIGHT	600
#define		FULLSCREEN_POSX		((FULLSCREEN_WIDTH - SCREEN_WIDTH) / 2)
#define		FULLSCREEN_POSY		((FULLSCREEN_HEIGHT - SCREEN_HEIGHT) / 2)

#define TOSTR(s) #s
#define _TOSTR(s) TOSTR(s)
#define PX68KVERSTR _TOSTR(PX68K_VERSION)


extern	uint8_t*	FONT;

extern	int32_t		HLINE_TOTAL;
extern	int32_t		VLINE_TOTAL;
extern	int32_t		VLINE;
extern	int32_t		vline;

extern	char	winx68k_dir[MAX_PATH];
extern	char	winx68k_ini[MAX_PATH];
extern	int32_t	BIOS030Flag;
extern	uint8_t	FrameChanged;

extern	int32_t	m68000_ICountBk;
extern	int32_t	ICount;

extern const char PrgTitle[];

#if defined(ANDROID) || TARGET_OS_IPHONE
extern int32_t realdisp_w, realdisp_h;
#endif

int32_t WinX68k_Reset(void);
void WinDraw_InitWindowSize(void);

#ifndef	winx68k_gtkwarpper_h
#define	winx68k_gtkwarpper_h

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <signal.h>

BOOL is_installed_idle_process(void);
void install_idle_process(void);
void uninstall_idle_process(void);

#define	NELEMENTS(array)	((int)(sizeof(array) / sizeof(array[0])))

#endif //winx68k_gtkwarpper_h

#endif //winx68k_wincore_h
