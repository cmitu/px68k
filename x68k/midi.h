#ifndef _winx68k_midi
#define _winx68k_midi

#include "common.h"

void MIDI_Init(void);
void MIDI_Cleanup(void);
void MIDI_Reset(void);
uint8_t FASTCALL MIDI_Read(int32_t adr);
void FASTCALL MIDI_Write(int32_t adr, uint8_t data);
void MIDI_SetModule(void);
void FASTCALL MIDI_Timer(int32_t clk);
int32_t MIDI_SetMimpiMap(char *filename);
int32_t MIDI_EnableMimpiDef(int32_t enable);
void MIDI_DelayOut(uint32_t delay);

#endif
