/* for WIN32-API */
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>

void
midOutChg(uint32_t port_no, uint32_t bank)
{
	/* select BANK CC20 LSB */
	uint32_t msg = ((bank << 16) | (0x20 << 8) | 0xb0);/*Bank select2*/
	midiOutShortMsg(hmo, msg);

 return;
}

#pragma comment(lib,"winmm.lib")
