// -----------------------------------------------------------------------
//   55.6/61fpsキープ用たいまー
// -----------------------------------------------------------------------
#include <unistd.h>
#include "common.h"
#include "crtc.h"
#include "mfp.h"
#include "winx68k.h"

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
	uint32_t ticknow = Get_usecCount();//1μs単位
	uint32_t dif;
	if(ticknow>tick){dif = ticknow-tick; }
	else{dif = 0xffffffff-tick+ticknow;}//補正

	uint32_t TIMEBASE;
	if(CRTC_Regs[0x29]&0x10){
	  TIMEBASE = VSYNC_HIGH * 567 / VLINE_TOTAL;//HSYNC:31KHz
	}
	else{
	  TIMEBASE = VSYNC_NORM * 283 / VLINE_TOTAL;//HSYNC:15.75KHz
	}
	if((TIMEBASE < (VSYNC_NORM - 5000)) || (TIMEBASE > (VSYNC_HIGH + 5000))){
	  TIMEBASE = ((CRTC_Regs[0x29]&0x10)?VSYNC_HIGH:VSYNC_NORM);
	}

	timercnt += dif*10;//0.1μs単位
	tick = ticknow;
	if ( timercnt>=TIMEBASE ) {
//		timercnt = 0;
		timercnt -= TIMEBASE;
		if ( timercnt>=(TIMEBASE*2) ) timercnt = 0;
		return 1;
	}
	else{
		if((TIMEBASE-timercnt)>5000){//over 500μs
		 usleep(50);//50μs sleep
		}
		return 0;
	}
}
