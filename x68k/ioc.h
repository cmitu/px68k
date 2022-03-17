#ifndef _winx68k_ioc
#define _winx68k_ioc

#include "common.h"

extern	uint8_t	IOC_IntStat;
extern	uint8_t	IOC_IntVect;

void IOC_Init(void);
uint8_t FASTCALL IOC_Read(int32_t adr);
void FASTCALL IOC_Write(int32_t adr, uint8_t data);

#endif
