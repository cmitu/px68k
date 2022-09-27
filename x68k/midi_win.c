/* for WIN32-API */
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include "prop.h"
#include "winui.h"
#include "midi.h"

extern HMIDIOUT hOut;
extern uint32_t midiOutGetNumDevs();
extern midiOutGetDeviceCapsA(uint32_t, LPMIDIOUTCAPS, uint32_t);

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
  /*Open MIDI port(MIDI_MAPPER)*/
  if (midiOutOpen(phmo, MIDI_MAPPER, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
  {
   midiOutReset(*phmo);
   /*Store MIDI port*/
   strcpy(menu_items[8][menu_no], "MIDI_MAPPER");
   menu_no++;
   strcpy(menu_items[8][menu_no],"\0"); /* Menu END */
  }

  /* What's No ? */
  num = midiOutGetNumDevs();

  for(devid = menu_no; devid<(num+menu_no); devid++){
    res=midiOutGetDeviceCapsA(devid, &OutCaps, sizeof(OutCaps));
    if(res == MMSYSERR_NOERROR){
     strcpy(menu_item[8][devid].OutCaps.szPname);
    }
  }
  strcpy(menu_item[8][devid]."\0");

 return devid;
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
	if(port_no == 0)
	{
	  if (midiOutOpen(&hOut, MIDI_MAPPER, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
	  {
	  midiOutReset(hOut);
	  }
	}
	else
	{
	  if (midiOutOpen(&hOut, (port_no-1), 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
	  {
	  midiOutReset(hOut);
	  }
	}

	/* select BANK CC20 LSB */
	msg = ((bank << 16) | (0x20 << 8) | 0xb0);/*Bank select2*/
	midiOutShortMsg(hmo, msg);

 return;
}

