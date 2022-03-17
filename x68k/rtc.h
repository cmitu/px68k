#ifndef _x68k_rtc
#define _x68k_rtc

void RTC_Init(void);
uint8_t FASTCALL RTC_Read(int32_t adr);
void FASTCALL RTC_Write(int32_t adr, uint8_t data);
void RTC_Timer(int32_t clock);

#endif
