// ---------------------------------------------------------------------------------------
//  TVRAM.C - Text VRAM
//  ToDo : 透明色処理とか色々
// ---------------------------------------------------------------------------------------

#include	"common.h"
#include	"winx68k.h"
#include	"windraw.h"
#include	"bg.h"
#include	"crtc.h"
#include	"palette.h"
#include	"m68000.h"
#include	"tvram.h"

	uint8_t	TVRAM[0x80000];
	uint8_t	TextDrawWork[1024*1024 + 100];
	uint8_t	TextDirtyLine[1024 + 100];

	uint8_t	TextDrawPattern[2048*4 +200];

//	WORD	Text_LineBuf[1024];	// →BGのを使うように変更
	uint8_t	Text_TrFlag[1024 + 100];

INLINE void TVRAM_WriteByteMask(int32_t adr, uint8_t data);

// -----------------------------------------------------------------------
//   全部書き換え〜
// -----------------------------------------------------------------------
void TVRAM_SetAllDirty(void)
{
	memset(TextDirtyLine, 1, 1024);
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void TVRAM_Init(void)
{
	uint32_t i, j;
	uint32_t bit;
	memset(TVRAM,  0, 0x80000);
	memset(TextDrawWork, 0, 1024*1024);
	TVRAM_SetAllDirty();

	memset(TextDrawPattern, 0, 2048*4);		// パターンテーブル初期化
	for (i=0; i<256; i++)
	{
		for (j=0, bit=0x80; j<8; j++, bit>>=1)
		{
			if (i&bit) {
				TextDrawPattern[i*8+j     ] = 1;
				TextDrawPattern[i*8+j+2048] = 2;
				TextDrawPattern[i*8+j+4096] = 4;
				TextDrawPattern[i*8+j+6144] = 8;
			}
		}
	}
}


// -----------------------------------------------------------------------
//   撤収
// -----------------------------------------------------------------------
void TVRAM_Cleanup(void)
{
}


// -----------------------------------------------------------------------
//   読むなり
// -----------------------------------------------------------------------
uint8_t FASTCALL TVRAM_Read(uint32_t adr)
{
	adr &= 0x7ffff;
#ifndef C68K_BIG_ENDIAN
	adr ^= 1;
#endif
	return TVRAM[adr];
}


// -----------------------------------------------------------------------
//   1ばいと書くなり
// -----------------------------------------------------------------------
INLINE void TVRAM_WriteByte(uint32_t adr, uint8_t data)
{
	adr &= 0x7ffff;
	if (TVRAM[adr]!=data)
	{
		TextDirtyLine[(((adr&0x1ffff)/128)-TextScrollY)&1023] = 1;
		TVRAM[adr] = data;
	}
}


// -----------------------------------------------------------------------
//   ますく付きで書くなり
// -----------------------------------------------------------------------
INLINE void TVRAM_WriteByteMask(int32_t adr, uint8_t data)
{
	data = (TVRAM[adr] & CRTC_Regs[0x2e + ((adr^1) & 1)]) | (data & (~CRTC_Regs[0x2e + ((adr ^ 1) & 1)]));
	if (TVRAM[adr] != data)
	{
		TextDirtyLine[(((adr&0x1ffff)/128)-TextScrollY)&1023] = 1;
		TVRAM[adr] = data;
	}
}


// -----------------------------------------------------------------------
//   書くなり
// -----------------------------------------------------------------------
void FASTCALL TVRAM_Write(uint32_t adr, uint8_t data)
{
	adr &= 0x7ffff;
#ifndef C68K_BIG_ENDIAN
	adr ^= 1;
#endif
	if (CRTC_Regs[0x2a]&1)			// 同時アクセス
	{
		adr &= 0x1ffff;
		if (CRTC_Regs[0x2a]&2)		// Text Mask
		{
			if (CRTC_Regs[0x2b]&0x10) TVRAM_WriteByteMask(adr        , data);
			if (CRTC_Regs[0x2b]&0x20) TVRAM_WriteByteMask(adr+0x20000, data);
			if (CRTC_Regs[0x2b]&0x40) TVRAM_WriteByteMask(adr+0x40000, data);
			if (CRTC_Regs[0x2b]&0x80) TVRAM_WriteByteMask(adr+0x60000, data);
		}
		else
		{
			if (CRTC_Regs[0x2b]&0x10) TVRAM_WriteByte(adr        , data);
			if (CRTC_Regs[0x2b]&0x20) TVRAM_WriteByte(adr+0x20000, data);
			if (CRTC_Regs[0x2b]&0x40) TVRAM_WriteByte(adr+0x40000, data);
			if (CRTC_Regs[0x2b]&0x80) TVRAM_WriteByte(adr+0x60000, data);
		}
	}
	else					// シングルアクセス
	{
		if (CRTC_Regs[0x2a]&2)		// Text Mask
		{
			TVRAM_WriteByteMask(adr, data);
		}
		else
		{
			TVRAM_WriteByte(adr, data);
		}
	}

	/* TV-RAM */
	int32_t *ptr = (int32_t *)TextDrawPattern;
	int32_t tvram_addr = adr & 0x1ffff;
	int32_t workadr = ((adr & 0x1ff80) + ((adr ^ 1) & 0x7f)) << 3;
	int32_t t0, t1;
	uint8_t pat;

	pat = TVRAM[tvram_addr + 0x60000];
	t0 = ptr[(pat * 2) + 1536];
	t1 = ptr[(pat * 2 + 1) + 1536];

	pat = TVRAM[tvram_addr + 0x40000];
	t0 |= ptr[(pat * 2) + 1024];
	t1 |= ptr[(pat * 2 + 1) + 1024];

	pat = TVRAM[tvram_addr + 0x20000];
	t0 |= ptr[(pat * 2) + 512];
	t1 |= ptr[(pat * 2 + 1) + 512];

	pat = TVRAM[tvram_addr];
	t0 |= ptr[(pat * 2)];
	t1 |= ptr[(pat * 2 + 1)];

	*((int32_t *)&TextDrawWork[workadr]) = t0;
	*(((int32_t *)(&TextDrawWork[workadr])) + 1) = t1;

}


// -----------------------------------------------------------------------
//   らすたこぴー時のあっぷでーと
// -----------------------------------------------------------------------
void FASTCALL TVRAM_RCUpdate(void)
{
	int32_t adr = ((int32_t)CRTC_Regs[0x2d]<<9);

	/* XXX: BUG */
	int32_t *ptr = (int32_t *)TextDrawPattern;
	int32_t *wptr = (int32_t *)(TextDrawWork + (adr << 3));
	int32_t t0, t1;
	int32_t tadr;
	uint8_t pat;
	int_fast16_t i;

	for (i = 0; i < 512; i++, adr++) {
		tadr = adr ^ 1;

		pat = TVRAM[tadr + 0x60000];
		t0 = ptr[(pat * 2) + 1536];
		t1 = ptr[(pat * 2 + 1) + 1536];

		pat = TVRAM[tadr + 0x40000];
		t0 |= ptr[(pat * 2) + 1024];
		t1 |= ptr[(pat * 2 + 1) + 1024];

		pat = TVRAM[tadr + 0x20000];
		t0 |= ptr[(pat * 2) + 512];
		t1 |= ptr[(pat * 2 + 1) + 512];

		pat = TVRAM[tadr];
		t0 |= ptr[(pat * 2)];
		t1 |= ptr[(pat * 2 + 1)];

		*wptr++ = t0;
		*wptr++ = t1;
	}
}

// -----------------------------------------------------------------------
//   1ライン描画
// -----------------------------------------------------------------------
void FASTCALL Text_DrawLine(int32_t opaq)
{
	int32_t addr;
	int32_t x, y;
	int32_t off = 16;
	int_fast32_t i;
	uint8_t t;

	y = TextScrollY + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y = (y & 0x3ff) << 10;

	x = TextScrollX & 0x3ff;
	addr = x + y;
	x = (x ^ 0x3ff) + 1;

	if (opaq) {
		for (i = 0; (i < TextDotX) && (x > 0); i++, x--, off++) {
			t = TextDrawWork[addr++] & 0xf;
			Text_TrFlag[off] = t ? 1 : 0;
			BG_LineBuf[off] = TextPal[t];
		}
		if (i++ != TextDotX) {
			for (; i < TextDotX; i++, off++) {
				BG_LineBuf[off] = TextPal[0];
				Text_TrFlag[off] = 0;
			}
		}
	} else {
		for (i = 0; (i < TextDotX) && (x > 0); i++, x--, off++) {
			t = TextDrawWork[addr++] & 0xf;
			if (t) {
				Text_TrFlag[off] |= 1;
				BG_LineBuf[off] = TextPal[t];
			}
		}
	}

}
