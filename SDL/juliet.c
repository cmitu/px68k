/*	$Id: juliet.c,v 1.2 2003/12/05 18:07:16 nonaka Exp $	*/

#include	"common.h"
#include	"juliet.h"

#if 0
/*
 * Juliet ダミー
 */

BOOL
juliet_load(void)
{

	return FAILURE;
}

void
juliet_unload(void)
{
}

BOOL
juliet_prepare(void)
{

	return FAILURE;
}


// ---- YM2151部
// リセットと同時に、OPMチップの有無も確認
void
juliet_YM2151Reset(void)
{
}

int32_t
juliet_YM2151IsEnable(void)
{

	return FALSE;
}

int32_t
juliet_YM2151IsBusy(void)
{

	return FALSE;
}


void
juliet_YM2151W(uint8_t reg, uint8_t data)
{
}

// ---- YMF288部

void
juliet_YMF288Reset(void)
{
}

int32_t
juliet_YM288IsEnable(void)
{

	return TRUE;
}

int32_t
juliet_YM288IsBusy(void)
{

	return 0;
}

void
juliet_YMF288A(uint8_t addr, uint8_t data)
{
}

void
juliet_YMF288B(uint8_t addr, uint8_t data)
{
}

void
juliet_YMF288W(uint8_t addr, uint8_t data)
{
}

uint8_t
juliet_YMF288R(uint8_t addr)
{

	return 0xff;
}

#endif
