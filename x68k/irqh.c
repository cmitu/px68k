// ---------------------------------------------------------------------------------------
//  IRQH.C - IRQ Handler (架空のデバイスにょ)
// ---------------------------------------------------------------------------------------


#include "../m68000/m68000.h"
#include "irqh.h"


	uint8_t	IRQH_IRQ[8];
	void	*IRQH_CallBack[8];

// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void IRQH_Init(void)
{
	memset(IRQH_IRQ, 0, 8);
}


// -----------------------------------------------------------------------
//   デフォルトのベクタを返す（これが起こったら変だお）
// -----------------------------------------------------------------------
int32_t FASTCALL IRQH_DefaultVector(uint8_t irq)
{
	IRQH_IRQCallBack(irq);
	return -1;
}


// -----------------------------------------------------------------------
//   他の割り込みのチェック
//   各デバイスのベクタを返すルーチンから呼ばれます
// -----------------------------------------------------------------------
void IRQH_IRQCallBack(uint8_t irq)
{
	IRQH_IRQ[irq&7] = 0;
	int_fast16_t i;

	m68000_set_irq_line(0);

	for (i=7; i>0; i--) // Hight to Low Priority
	{
	    if (IRQH_IRQ[i])
	    {
			m68000_set_irq_line(i);
			return;
	    }
	}
}

// -----------------------------------------------------------------------
//   割り込み発生
// -----------------------------------------------------------------------
void IRQH_Int(uint8_t irq, void* handler)
{
	int_fast16_t i;

	IRQH_IRQ[irq&7] = 1;

	if (handler==NULL)
	    IRQH_CallBack[irq&7] = &IRQH_DefaultVector;
	else
	    IRQH_CallBack[irq&7] = handler;

	for (i=7; i>0; i--)
	{
	    if (IRQH_IRQ[i])
	    {
	        m68000_set_irq_line(i);
	        return;
	    }
	}

}

int32_t  my_irqh_callback(int32_t  level)
{
    int_fast16_t i;

    C68K_INT_CALLBACK *func = IRQH_CallBack[level&7];
    int32_t vect = (func)(level&7);
    //p6logd("irq vect = %x line = %d\n", vect, level);

    for (i=7; i>0; i--)
    {
		if (IRQH_IRQ[i])
		{
	    	m68000_set_irq_line(i);
			break;
		}
    }

    return (int32_t)vect;
}
