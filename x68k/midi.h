#ifndef _winx68k_midi
#define _winx68k_midi

#include "../win32api/mmsystem.h"

void MIDI_Init(void);
void MIDI_Cleanup(void);
void MIDI_Reset(void);
uint8_t FASTCALL MIDI_Read(uint32_t adr);
void FASTCALL MIDI_Write(uint32_t adr, uint8_t data);
void MIDI_SetModule(void);
void FASTCALL MIDI_Timer(int32_t clk);
int32_t MIDI_SetMimpiMap(char *filename);
int32_t MIDI_EnableMimpiDef(int32_t enable);
void MIDI_DelayOut(uint32_t delay);

extern uint32_t midiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh);
extern uint32_t midiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh);
extern uint32_t midiOutShortMsg(HMIDIOUT hmo, uint32_t dwMsg);
extern uint32_t midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh);
extern uint32_t midiOutOpen(LPHMIDIOUT phmo, uint32_t uDeviceID, uint32_t dwCallback, uint32_t dwInstance, uint32_t fdwOpen);
extern uint32_t midiOutClose(HMIDIOUT hmo);
extern uint32_t midiOutReset(HMIDIOUT hmo);
extern uint32_t mid_DevList(LPHMIDIOUT phmo);
extern void midOutChg(uint32_t mid_out_port, uint32_t bank);

#endif
