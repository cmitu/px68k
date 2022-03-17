// -----------------------------------------------------------------------
//   55.6fpsキープ用たいまー
// -----------------------------------------------------------------------
#include "common.h"
#include "crtc.h"
#include "mfp.h"

uint32_t	timercnt = 0;
uint32_t	tick = 0;

void Timer_Init(void)
{
	tick = timeGetTime();
}

void Timer_Reset(void)
{
	tick = timeGetTime();
}

uint16_t Timer_GetCount(void)
{
	uint32_t ticknow = timeGetTime();
	uint32_t dif = ticknow-tick;
	uint32_t TIMEBASE = ((CRTC_Regs[0x29]&0x10)?VSYNC_HIGH:VSYNC_NORM);

	timercnt += dif*10000;
	tick = ticknow;
	if ( timercnt>=TIMEBASE ) {
//		timercnt = 0;
		timercnt -= TIMEBASE;
		if ( timercnt>=(TIMEBASE*2) ) timercnt = 0;
		return 1;
	} else
		return 0;
}
