/* for WIN32-API */
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include "prop.h"
#include "winui.h"
#include "midi.h"

#pragma comment(lib,"winmm.lib")

/*
  Listing MIDI device List
*/
uint32_t
mid_DevList()
{
  MIDIOUTCAPS OutCaps;
  MMRESULT res;
  uint32_t num, devid;

  /* What's No ? */
  num = midiOutGetNumDevs();

  for (devid=0; devid<num; devid++) {
	/* Get Device information */
	res = midiOutGetDevCaps(devid, &OutCaps, sizeof(OutCaps));
	if (res == MMSYSERR_NOERROR) {
	 //printf("ID=%d: %s\n", devid, outCaps.szPname);
	 strcpy(menu_items[8][devid], OutCaps.szPname);
	}
  }

  strcpy(menu_items[8][devid],"\0"); /* Menu END */

 return num;
}

/*
  Select Port and BANK
*/
void
midOutChg(uint32_t port_no, uint32_t bank)
{
	HMIDIOUT hmo;

	/* All note off */
	for (uint32_t msg=0x7bb0; msg<0x7bc0; msg++) {
		midiOutShortMsg(hmo, msg);
	}

	/*Open MIDI port*/
	if (midiOutOpen(&hOut, port_no, 0, 0, CALLBACK_NULL)
							== MMSYSERR_NOERROR) {
	  midiOutReset(hOut);
	}

	/* select BANK CC20 LSB */
	uint32_t msg = ((bank << 16) | (0x20 << 8) | 0xb0);/*Bank select2*/
	midiOutShortMsg(hmo, msg);

 return;
}

