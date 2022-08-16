/******************************************************************************

	m68000.c

	M68000 CPUインタフェース関数

******************************************************************************/

#include "m68000.h"
#include "c68k/c68k.h"
#include "../x68k/x68kmemory.h"

int32_t m68000_ICountBk;
int32_t ICount;

/******************************************************************************
	M68000インタフェース関数
******************************************************************************/

/*--------------------------------------------------------
	CPU初期化
--------------------------------------------------------*/
int32_t my_irqh_callback(int32_t level);

void m68000_init(void)
{
    C68k_Init(&C68K, my_irqh_callback);

    C68k_Set_ReadB(&C68K, Memory_ReadB);
    C68k_Set_ReadW(&C68K, Memory_ReadW);
    C68k_Set_WriteB(&C68K, Memory_WriteB);
    C68k_Set_WriteW(&C68K, Memory_WriteW);

        C68k_Set_Fetch(&C68K, 0x000000, 0xbfffff, (uintptr_t)MEM);
        C68k_Set_Fetch(&C68K, 0xc00000, 0xc7ffff, (uintptr_t)GVRAM);
        C68k_Set_Fetch(&C68K, 0xe00000, 0xe7ffff, (uintptr_t)TVRAM);
        C68k_Set_Fetch(&C68K, 0xea0000, 0xea1fff, (uintptr_t)SCSIIPL);
        C68k_Set_Fetch(&C68K, 0xed0000, 0xed3fff, (uintptr_t)SRAM);
        C68k_Set_Fetch(&C68K, 0xf00000, 0xfbffff, (uintptr_t)FONT);
        C68k_Set_Fetch(&C68K, 0xfc0000, 0xffffff, (uintptr_t)IPL);
}


/*--------------------------------------------------------
	CPUリセット
--------------------------------------------------------*/

void m68000_reset(void)
{
	C68k_Reset(&C68K);
}


/*--------------------------------------------------------
	CPU停止
--------------------------------------------------------*/

void m68000_exit(void)
{
}


/*--------------------------------------------------------
	CPU実行
--------------------------------------------------------*/

int32_t m68000_execute(int32_t cycles)
{
	return C68k_Exec(&C68K, cycles);
}


/*--------------------------------------------------------
	割り込み処理
--------------------------------------------------------*/
//void m68000_set_irq_line(int32_t irqline, int32_t state)
void m68000_set_irq_line(int32_t irqline)
{
//	if (irqline == IRQ_LINE_NMI)
//		irqline = 7;

//	C68k_Set_IRQ(&C68K, irqline, state);
	C68k_Set_IRQ(&C68K, irqline);
}


/*--------------------------------------------------------
	割り込みコールバック関数設定
--------------------------------------------------------*/

void m68000_set_irq_callback(int32_t (*callback)(int32_t line))
{
//	C68k_Set_IRQ_Callback(&C68K, callback);
}


/*--------------------------------------------------------
	レジスタ取得
--------------------------------------------------------*/

uint32_t m68000_get_reg(int32_t regnum)
{
	switch (regnum)
	{

	case M68K_PC:  return C68k_Get_PC(&C68K);
	case M68K_USP: return C68k_Get_USP(&C68K);
	case M68K_MSP: return C68k_Get_MSP(&C68K);
	case M68K_SR:  return C68k_Get_SR(&C68K);
	case M68K_D0:  return C68k_Get_DReg(&C68K, 0);
	case M68K_D1:  return C68k_Get_DReg(&C68K, 1);
	case M68K_D2:  return C68k_Get_DReg(&C68K, 2);
	case M68K_D3:  return C68k_Get_DReg(&C68K, 3);
	case M68K_D4:  return C68k_Get_DReg(&C68K, 4);
	case M68K_D5:  return C68k_Get_DReg(&C68K, 5);
	case M68K_D6:  return C68k_Get_DReg(&C68K, 6);
	case M68K_D7:  return C68k_Get_DReg(&C68K, 7);
	case M68K_A0:  return C68k_Get_AReg(&C68K, 0);
	case M68K_A1:  return C68k_Get_AReg(&C68K, 1);
	case M68K_A2:  return C68k_Get_AReg(&C68K, 2);
	case M68K_A3:  return C68k_Get_AReg(&C68K, 3);
	case M68K_A4:  return C68k_Get_AReg(&C68K, 4);
	case M68K_A5:  return C68k_Get_AReg(&C68K, 5);
	case M68K_A6:  return C68k_Get_AReg(&C68K, 6);
	case M68K_A7:  return C68k_Get_AReg(&C68K, 7);

	default: return 0;
	}
}


/*--------------------------------------------------------
	レジスタ設定
--------------------------------------------------------*/

void m68000_set_reg(int32_t regnum, uint32_t val)
{
	switch (regnum)
	{

	case M68K_PC:  C68k_Set_PC(&C68K, val); break;
	case M68K_USP: C68k_Set_USP(&C68K, val); break;
	case M68K_MSP: C68k_Set_MSP(&C68K, val); break;
	case M68K_SR:  C68k_Set_SR(&C68K, val); break;
	case M68K_D0:  C68k_Set_DReg(&C68K, 0, val); break;
	case M68K_D1:  C68k_Set_DReg(&C68K, 1, val); break;
	case M68K_D2:  C68k_Set_DReg(&C68K, 2, val); break;
	case M68K_D3:  C68k_Set_DReg(&C68K, 3, val); break;
	case M68K_D4:  C68k_Set_DReg(&C68K, 4, val); break;
	case M68K_D5:  C68k_Set_DReg(&C68K, 5, val); break;
	case M68K_D6:  C68k_Set_DReg(&C68K, 6, val); break;
	case M68K_D7:  C68k_Set_DReg(&C68K, 7, val); break;
	case M68K_A0:  C68k_Set_AReg(&C68K, 0, val); break;
	case M68K_A1:  C68k_Set_AReg(&C68K, 1, val); break;
	case M68K_A2:  C68k_Set_AReg(&C68K, 2, val); break;
	case M68K_A3:  C68k_Set_AReg(&C68K, 3, val); break;
	case M68K_A4:  C68k_Set_AReg(&C68K, 4, val); break;
	case M68K_A5:  C68k_Set_AReg(&C68K, 5, val); break;
	case M68K_A6:  C68k_Set_AReg(&C68K, 6, val); break;
	case M68K_A7:  C68k_Set_AReg(&C68K, 7, val); break;

	default: break;
	}
}


/*------------------------------------------------------
	セーブ/ロード ステート
------------------------------------------------------*/

#ifdef SAVE_STATE

STATE_SAVE( m68000 )
{
	int i;
	UINT32 pc = C68k_Get_Reg(&C68K, C68K_PC);

	for (i = 0; i < 8; i++)
		state_save_long(&C68K.D[i], 1);
	for (i = 0; i < 8; i++)
		state_save_long(&C68K.A[i], 1);

	state_save_long(&C68K.flag_C, 1);
	state_save_long(&C68K.flag_V, 1);
	state_save_long(&C68K.flag_Z, 1);
	state_save_long(&C68K.flag_N, 1);
	state_save_long(&C68K.flag_X, 1);
	state_save_long(&C68K.flag_I, 1);
	state_save_long(&C68K.flag_S, 1);
	state_save_long(&C68K.USP, 1);
	state_save_long(&pc, 1);
	state_save_long(&C68K.HaltState, 1);
	state_save_long(&C68K.IRQLine, 1);
	state_save_long(&C68K.IRQState, 1);
}

STATE_LOAD( m68000 )
{
	int i;
	UINT32 pc;

	for (i = 0; i < 8; i++)
		state_load_long(&C68K.D[i], 1);
	for (i = 0; i < 8; i++)
		state_load_long(&C68K.A[i], 1);

	state_load_long(&C68K.flag_C, 1);
	state_load_long(&C68K.flag_V, 1);
	state_load_long(&C68K.flag_Z, 1);
	state_load_long(&C68K.flag_N, 1);
	state_load_long(&C68K.flag_X, 1);
	state_load_long(&C68K.flag_I, 1);
	state_load_long(&C68K.flag_S, 1);
	state_load_long(&C68K.USP, 1);
	state_load_long(&pc, 1);
	state_load_long(&C68K.HaltState, 1);
	state_load_long(&C68K.IRQLine, 1);
	state_load_long(&C68K.IRQState, 1);

	C68k_Set_Reg(&C68K, C68K_PC, pc);
}

#endif /* SAVE_STATE */
