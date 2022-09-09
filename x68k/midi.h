#ifndef _winx68k_midi
#define _winx68k_midi

#include "common.h"

typedef struct midihdr {
	char*			lpData;
	uint32_t			dwBufferLength;
	uint32_t			dwBytesRecorded;
	uint32_t			dwUser;
	uint32_t			dwFlags;
	struct midihdr *	lpNext;
	uint32_t			reserved;
	uint32_t			dwOffset;
	uint32_t			dwReserved[8];
} MIDIHDR, *PMIDIHDR, *NPMIDIHDR, *LPMIDIHDR;

typedef	HANDLE		HMIDIOUT;
typedef	HMIDIOUT *	LPHMIDIOUT;

#define MIDBUF_SIZE 200
#define	MMSYSERR_NOERROR	0
#define	MIDIERR_STILLPLAYING	2
#define	MIDI_MAPPER		-1
#define	CALLBACK_NULL		0x00000000L

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
extern void midOutChg(uint32_t mid_out_port, uint32_t bank);

#endif
