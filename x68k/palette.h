#ifndef _winx68k_pal
#define _winx68k_pal

#include "common.h"

extern	uint8_t		Pal_Regs[1024];
extern	uint8_t		Contrast_Value;

extern	uint32_t	TextPal32[256];
extern	uint32_t	GrphPal32[256];
extern	uint32_t	Pal32[65536];

void Pal_SetColor(void);
void Pal_Init(void);

uint8_t FASTCALL Pal_Read(int32_t adr);
uint16_t FASTCALL Pal_Read16(int32_t adr);
void FASTCALL Pal_Write(int32_t adr, uint8_t data);
void FASTCALL Pal_Write16(int32_t adr, uint16_t data);

void Pal_TrackContrast(void);
void Pal_ChangeContrast(int32_t num);
void Pal32_ChangeContrast(int32_t num);

//extern uint16_t Ibit, Pal_HalfMask, Pal_Ix2;
extern uint32_t Ibit32, Abit32, Pal32_X68kMask, Pal32_FullMask;

#endif

