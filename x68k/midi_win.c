/* for WIN32-API */
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include "prop.h"
#include "winui.h"
#include "midi.h"

extern HMIDIOUT hOut;
extern uint32_t midiOutGetNumDevs();
extern uint32_t midiOutGetDevCapsA(uint32_t, LPMIDIOUTCAPS, uint32_t);

#pragma comment(lib,"winmm.lib")

/*
  Listing MIDI device List
*/
uint32_t
mid_DevList(LPHMIDIOUT phmo)
{
  MIDIOUTCAPS OutCaps;
  MMRESULT res;
  uint32_t num, devid, menu_no;

  menu_no=0;

  /* What's No ? */
  num = midiOutGetNumDevs();

  for(devid = 0; devid<num; devid++){
    res=midiOutGetDevCapsA(devid, &OutCaps, sizeof(OutCaps));
    if(res == MMSYSERR_NOERROR){
     strcpy(menu_items[8][menu_no], OutCaps.szPname);
     menu_no++;
    }
  }
  strcpy(menu_items[8][menu_no], "\0");

 return menu_no;
}

/*
  Select Port and BANK
*/
void
midOutChg(uint32_t port_no, uint32_t bank)
{
	HMIDIOUT hmo;
	uint32_t msg;

	/* All note off */
	for (msg=0x7bb0; msg<0x7bc0; msg++)
	{
	  midiOutShortMsg(hmo, msg);
	}

	midiOutClose(hOut);

	if (midiOutOpen(&hOut, (port_no), 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
	{
	  midiOutReset(hOut);
	}

	/* select BANK CC20 LSB */
	msg = ((bank << 16) | (0x20 << 8) | 0xb0);/*Bank select2*/
	midiOutShortMsg(hmo, msg);

 return;
}

