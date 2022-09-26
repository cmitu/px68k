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
mid_DevList(LPHMIDIOUT phmo)
{

  uint32_t num;

  /* What's No ? */
  num = midiOutGetNumDevs();

  /*Open MIDI port(MIDI_MAPPER)*/
  if (midiOutOpen(phmo, MIDI_MAPPER, 0, 0, CALLBACK_NULL)
                == MMSYSERR_NOERROR) {
   midiOutReset(*phmo);
   /*Store MIDI port*/
   strcpy(menu_items[8][0], "MIDI_MAPPER");
   strcpy(menu_items[8][1],"\0"); /* Menu END */
  }

 return 1;
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
	for (msg=0x7bb0; msg<0x7bc0; msg++) {
	  midiOutShortMsg(hmo, msg);
	}

	/* select BANK CC20 LSB */
	msg = ((bank << 16) | (0x20 << 8) | 0xb0);/*Bank select2*/
	midiOutShortMsg(hmo, msg);

 return;
}

