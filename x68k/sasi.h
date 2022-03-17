#ifndef _winx68k_sasi
#define _winx68k_sasi

#include "common.h"

void SASI_Init(void);
uint8_t FASTCALL SASI_Read(int32_t adr);
void FASTCALL SASI_Write(int32_t adr, uint8_t data);
int32_t SASI_IsReady(void);

extern char SASI_Name[16][MAX_PATH];

#endif //_winx68k_sasi
