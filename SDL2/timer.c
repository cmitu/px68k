// -----------------------------------------------------------------------
//   55.6fpsキープ用たいまー
// -----------------------------------------------------------------------
#include <unistd.h>
#include "common.h"
#include "crtc.h"
#include "mfp.h"

uint32_t	timercnt = 0;
uint32_t	tick = 0;

void Timer_Init(void)
{
	tick = Get_usecCount();
}

void Timer_Reset(void)
{
	tick = Get_usecCount();
}

uint16_t Timer_GetCount(void)
{
	uint32_t ticknow = Get_usecCount();//0.1μs単位
	uint32_t TIMEBASE = ((CRTC_Regs[0x29]&0x10)?VSYNC_HIGH:VSYNC_NORM);
	uint32_t dif;
	if(ticknow>tick){dif = ticknow-tick; }
	else{dif = 1000000000-tick+ticknow;}//100秒周期補正

	timercnt += dif;//0.1μs単位
	tick = ticknow;
	if ( timercnt>=TIMEBASE ) {
//		timercnt = 0;
		timercnt -= TIMEBASE;
		if ( timercnt>=(TIMEBASE*2) ) timercnt = 0;
		return 1;
	}
	else{
		if((TIMEBASE-timercnt)>8000){//over 800μs
		 usleep(100);//100μs sleep
		}
		return 0;
	}
}
