#ifndef _winx68k_adpcm_h
#define _winx68k_adpcm_h

extern uint8_t ADPCM_Clock;
extern int32_t ADPCM_ClockRate;

void FASTCALL ADPCM_PreUpdate(int32_t clock);
void FASTCALL ADPCM_Update(int16_t *buffer, int32_t length, int32_t rate, uint8_t *pbsp, uint8_t *pbep);

void FASTCALL ADPCM_Write(int32_t adr, uint8_t data);
uint8_t FASTCALL ADPCM_Read(int32_t adr);

void ADPCM_SetVolume(uint8_t vol);
void ADPCM_SetPan(int32_t n);
void ADPCM_SetClock(int32_t n);

void ADPCM_Init(int32_t samplerate);
int32_t ADPCM_IsReady(void);

#endif
