#ifndef _winx68k_sysport
#define _winx68k_sysport

#include "common.h"

extern	uint8_t	SysPort[7];

void SysPort_Init(void);
uint8_t FASTCALL SysPort_Read(int32_t adr);
void FASTCALL SysPort_Write(int32_t adr, uint8_t data);

#endif
