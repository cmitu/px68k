// ---------------------------------------------------------------------------------------
//  PIA.C - uPD8255（必要最低限のみ）
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "pia.h"
#include "adpcm.h"
#include "m68000.h"

extern uint8_t FASTCALL Joystick_Read();
extern void FASTCALL Joystick_Write();

typedef struct {
	uint8_t PortA;
	uint8_t PortB;
	uint8_t PortC;
	uint8_t Ctrl;
} PIA;

static PIA pia;

// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void PIA_Init(void)
{
	pia.PortA = 0xff;
	pia.PortB = 0xff;
	pia.PortC = 0x0b;
	pia.Ctrl = 0;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL PIA_Write(uint32_t adr, uint8_t data)
{
	uint8_t mask, bit, portc = pia.PortC;

	switch(adr){
	case 0xe9a001://PortA
		pia.PortA = data;
		break;
	case 0xe9a003://PortB
		pia.PortB = data;
		break;
	case 0xe9a005://PortC
		portc = pia.PortC;
		pia.PortC = data;
		if ( (portc&0x0f)!=(pia.PortC&0x0f) ) ADPCM_SetPan(pia.PortC&0x0f);
		if ( (portc&0x10)!=(pia.PortC&0x10) ) Joystick_Write(0, (uint8_t)((data&0x10)?0xff:0x00));
		if ( (portc&0x20)!=(pia.PortC&0x20) ) Joystick_Write(1, (uint8_t)((data&0x20)?0xff:0x00));
		break;
	case 0xe9a007://Control
		if ( !(data&0x80) ) {
			portc = pia.PortC;
			bit = (data>>1)&7;// bit NO.
			mask = 1<<bit;
			if ( data&1 )    //Set/Reset
				pia.PortC |= mask;
			else
				pia.PortC &= ~mask;
			if ( (portc&0x0f)!=(pia.PortC&0x0f) ) ADPCM_SetPan(pia.PortC&0x0f);
			if ( (portc&0x10)!=(pia.PortC&0x10) ) Joystick_Write(0, (uint8_t)((data&1)?0xff:0x00));
			if ( (portc&0x20)!=(pia.PortC&0x20) ) Joystick_Write(1, (uint8_t)((data&1)?0xff:0x00));
		}
		else{
			pia.Ctrl = data;
		}
		break;
	default:
		break;
	}

}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
uint8_t FASTCALL PIA_Read(uint32_t adr)
{
	uint8_t ret=0xff;

	switch(adr){
	case 0xe9a001://PortA
		if(pia.Ctrl & 0x10) ret = Joystick_Read(0);
		else ret = pia.PortA;
		break;
	case 0xe9a003://PortB
		if(pia.Ctrl & 0x02) ret = Joystick_Read(1);
		else ret = pia.PortB;
		break;
	case 0xe9a005://PortC
		ret = pia.PortC;
		break;
	case 0xe9a007://Control
		ret = pia.Ctrl;
		break;
	default:
		break;
	}

	return ret;
}
