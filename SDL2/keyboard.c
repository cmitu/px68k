/* 
 * Copyright (c) 2003,2008 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SDL1
#include "SDL2/SDL.h"
#else
#include "SDL/SDL.h"
#endif

#include "common.h"
#include "joystick.h"
#include "prop.h"
#include "keyboard.h"
#include "mfp.h"
#include "windraw.h"

uint8_t	KeyBufWP;
uint8_t	KeyBufRP;
uint8_t	KeyBuf[KeyBufSize];
uint8_t	KeyEnable = 1;
uint8_t	KeyIntFlag = 0;

struct keyboard_key kbd_key[] = {
#include "keytbl.inc"
};

extern uint8_t traceflag;

#if defined(ANDROID) || TARGET_OS_IPHONE
// キーボードの座標
int32_t kbd_x = 800, kbd_y = 0, kbd_w = 766, kbd_h = 218;
#endif

void
Keyboard_Init(void)
{

	KeyBufWP = 0;
	KeyBufRP = 0;
	memset(KeyBuf, 0, KeyBufSize);
	KeyEnable = 1;
	KeyIntFlag = 0;

}

// ----------------------------------
//	てーぶる類
// ----------------------------------

#define	NC	0
#define KEYTABLE_MAX 512

// 2022/5/8  XKBは削除 SDL1/2専用テーブルに作り換え
// SDL1/2共通キーコード 0~177(0xB1)変換テーブル
uint8_t KeyTable[KEYTABLE_MAX] = {
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	  BS, TAB,    ,    ,    , RET,    ,    		; 0x08
		0x0f,0x10,  NC,  NC,  NC,0x1d,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x10
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    , ESC,    ,    ,    ,    		; 0x18
		  NC,  NC,  NC,0x01,  NC,  NC,  NC,  NC,
	//	 SPC,  ! ,  " ,  # ,  $ ,  % ,  & ,  '		; 0x20
		0x35,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	//	  ( ,  ) ,  * ,  + ,  , ,  - ,  . ,  /		; 0x28
		0x09,0x0a,0x28,0x27,0x31,0x0c,0x32,0x33,
	//	  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7		; 0x30
		0x0b,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	//	  8 ,  9 ,  ; ,  : ,  < ,  = ,  > ,  ? 		; 0x38
		0x09,0x0a,0x28,0x27,0x31,0x0c,0x32,0x33,
	//	  @ ,  A ,  B ,  C ,  D ,  E ,  F ,  G		; 0x40
		0x1b,0x1e,0x2e,0x2c,0x20,0x13,0x21,0x22,
	//	  H ,  I ,  J ,  K ,  L ,  M ,  N ,  O		; 0x48
		0x23,0x18,0x24,0x25,0x26,0x30,0x2f,0x10,
	//	  P ,  Q ,  R ,  S ,  T ,  U ,  V ,  W		; 0x50
		0x1a,0x11,0x14,0x1f,0x15,0x17,0x2d,0x12,
	//	  X ,  Y ,  Z ,  [ ,  \ ,  ] ,  ^ ,  _		; 0x58
		0x2b,0x16,0x2a,0x1c,0x0e,0x29,0x0d,0x34,
	//	  ` ,  a ,  b ,  c ,  d ,  e ,  f ,  g		; 0x60
		0x1b,0x1e,0x2e,0x2c,0x20,0x13,0x21,0x22,
	//	  h ,  i ,  j ,  k ,  l ,  m ,  n ,  o		; 0x68
		0x23,0x18,0x24,0x25,0x26,0x30,0x2f,0x19,
	//	  p ,  q ,  r ,  s ,  t ,  u ,  v ,  w		; 0x70
		0x1a,0x11,0x14,0x1f,0x15,0x17,0x2d,0x12,
	//	  x ,  y ,  z ,  { ,  | ,  } ,  ~ ,   		; 0x78
		0x2b,0x16,0x2a,0x1c,0x0e,0x29,0x0d,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x80
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,   ¥,    ,    ,    ,   ¥,    ,    		; 0xa0(JIS Keyboard)
		  NC,0x0e,  NC,  NC,  NC,0x0e,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xa8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xb8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,


	// ==== 以下SDL1専用 特殊キー・テーブル

	//	   0,   1,   2,   3,   4,   5,   6,   7		; 0x100 (10key)
		0x4f,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x43,
	//	   8,   9,   .,   /,   *,   -,   +, ent		; 0x108
		0x44,0x45,0x51,0x40,0x41,0x42,0x46,0x4e,
	//	   =,  ⬆︎,   ⬇︎,  ➡︎,  ⬅︎,    ,    ,   		; 0x110
		0x4a,0x3c,0x3e,0x3d,0x3b,  NC,  NC,  NC,
	//	    ,    ,  F1,  F2,  F3,  F4,  F5, F6		; 0x118
		  NC,  NC,0x63,0x64,0x65,0x66,0x67,0x68,
	//	  F7,  F8,  F9, F10, F11, F12,    ,    		; 0x120
		0x69,0x6a,0x6b,0x6c,0x5a,0x5c,  NC,  NC,
	//	    ,    ,    ,    ,    ,CAPS,    , RFTL	; 0x128
		  NC,  NC,  NC,  NC,  NC,0x5d,  NC, 0x70,
	//	SFTL,    ,CTRL,    , ALT,RCMD,LCMD,    		; 0x130
		0x70,  NC,0x71,  NC,0x55,0x72,0x5f,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x138
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x140
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x148
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x150
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x158
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x160
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x168
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x170
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x178
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x180
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x188
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x190
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,     	; 0x198
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1a0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1a8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1b0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1b8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1c0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1c8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1d0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x1d8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x1e0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x1e8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1f0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x1f8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC
};

#define KEYTABLE_MAX2 240

//SDL2専用  特殊キー・テーブル 0x40000000~(0x400000E7)までの変換テーブル
uint8_t KeyTable4[KEYTABLE_MAX2] = {
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x08
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x10
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x18
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x20
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x28
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x30
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,CAPS,  F1,  F2,  F3,  F4,  F5,  F6		; 0x38
		  NC,0x5d,0x63,0x64,0x65,0x66,0x67,0x68,
	//	  F7,  F8,  F9, F10, F11, F12,PSCR,SCRL		; 0x40
		0x69,0x6a,0x6b,0x6c,0x5a,0x5b,0x62,0x53,
	//	PAUS, INS,HOME, PUP,    , END,PDWN,  ➡︎	; 0x48
		0x61,0x5e,0x36,0x38,  NC,0x3a,0x39,0x3d,
	//	  ⬅︎,  ⬇︎,  ⬆︎, NUM,   /,   *,   -,   +		; 0x50(10key)
		0x3b,0x3e,0x3c,0x3f,0x40,0x41,0x42,0x46,
	//	 ENT,   1,   2,   3,   4,   5,   6,   7		; 0x58
		0x4e,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x43,
	//	   8,   9,   0,   .,    ,    ,    ,   =		; 0x60
		0x44,0x45,0x4f,0x51,  NC,  NC,  NC,0x4a,
	//	 F13, F14, F15,    ,    ,    ,    ,    		; 0x68
		0x5c,0x52,0x54,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x78
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,   ,,    ,    		; 0x80
		  NC,  NC,  NC,  NC,  NC,0x50,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xa8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	CTRL, SFT, ALT, CMD,CTRL, SFT, ALT, CMD 	; 0xe0
		0x71,0x70,0x55,0x5f,0x71,0x70,0x59,0x72,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
};


// P6K: PX68K_KEYBOARD
//      ~ ~   ~
#define P6K_UP 1
#define P6K_DOWN 2

void send_keycode(uint8_t code, int32_t flag)
{
	uint8_t newwp;

	if (code != NC) {
		newwp = ((KeyBufWP + 1) & (KeyBufSize - 1));
		if (newwp != KeyBufRP) {
			KeyBuf[KeyBufWP] = code | ((flag == P6K_UP)? 0x80 : 0);
			//p6logd("KeyBufWP:%d KeyBuf[]:%x\n", KeyBufWP,KeyBuf[KeyBufWP]);
			KeyBufWP = newwp;
			//p6logd("KeyBufWP: %d\n", KeyBufWP);
		}
	}
}

static int32_t shiftdown = 0;

static int32_t get_x68k_keycode(uint32_t wp)
{

	if(Config.KeyboardType==1){/* US-Keyboard */
	 switch (wp) {
	 case SDLK_2:/*2*/
		  if(shiftdown==1){	/*Shift*/
		  send_keycode(0x70, P6K_UP);
		  return 0x1b;			/*@*/
		  }
		break;
	 case SDLK_6:/*6*/
		  if(shiftdown==1){	/*Shift*/
		  send_keycode(0x70, P6K_UP);
		  return 0x0d;			/*^*/
		  }
		break;
	 case SDLK_7:/*7*/
		  if(shiftdown==1)	return 0x07;		/*&*/
		break;
	 case SDLK_8:/*8*/
		  if(shiftdown==1)	return 0x28;		/* * */
		break;
	 case SDLK_9:/*9*/
		  if(shiftdown==1)	return 0x09;		/*(*/
		break;
	 case SDLK_0:/*0*/
		  if(shiftdown==1)	 return 0x0a;		/*)*/
		break;
	 case SDLK_MINUS:/*-*/
		  if(shiftdown==1)	 return 0x34;		/*_*/
		break;
	 case SDLK_CARET:/*^*/
		  if(shiftdown==1)	 return 0x27;		/*+*/
		  else{
			send_keycode(0x70, P6K_DOWN);
			return 0x0c;						/*=*/
		  }
		break;
	 case SDLK_AT:/*@*/
			return 0x1c;						/*[*/
		break;
	 case SDLK_LEFTBRACKET:/*[*/
			return 0x29;						/*]*/
		break;
	 case SDLK_RIGHTBRACKET:/*[*/
			return 0x0e;						/*|*/
		break;
	 case SDLK_SEMICOLON:/*;*/
		  if(shiftdown==1){
			send_keycode(0x70, P6K_UP);
			return 0x28;						/*:*/
		  }
		break;
	 case SDLK_COLON:/*'*/
		  if(shiftdown==1)	 return 0x03;		/*"*/
		  else{
			send_keycode(0x70, P6K_DOWN);
			return 0x08;						/*'*/
		  }
		break;
	 case SDLK_BACKQUOTE:/*`*/
		  if(shiftdown==1)	 return 0x0d;		/*~*/
		  else{
			send_keycode(0x70, P6K_DOWN);
			return 0x1b;						/*`*/
		  }
		break;
	 }
	}


	if (wp < KEYTABLE_MAX) { 		//SDL1/2キーコード (SDL1:0x100~)
		return KeyTable[wp];
	}

	if((wp & 0x40000000) != 0){
	  uint32_t wp2 = (wp & 0x000001FF); //SDL2キーコード 0x40000000以上
		if(wp2 < KEYTABLE_MAX2){
		  return KeyTable4[wp2];
		}
	}

return -1;

}

// ----------------------------------
//	WM_KEYDOWN〜
// ----------------------------------
void
Keyboard_KeyDown(uint32_t wp)
{

	uint8_t code;
	uint8_t newwp;
#if 0
	if (wp & ~0xff) {
		if (wp == GDK_VoidSymbol)
			code = NC;
		else if ((wp & 0xff00) == 0xff00)
			code = KeyTable[(wp & 0xff) | 0x100];
		else
			code = NC;
	} else
		code = KeyTable[wp & 0xff];
#endif
	code = get_x68k_keycode(wp);
	if (code < 0) {
		return;
	}
	if(code == 0x70){ shiftdown = 1; }

	//printf("Keyboard_KeyDown: ");
	//printf("wp=0x%x, code=0x%x\n", wp, code);
	//printf("SDLK_UP: 0x%x\n", SDLK_UP);

#if 0
	if (code != NC) {
		newwp = ((KeyBufWP + 1) & (KeyBufSize - 1));
		if (newwp != KeyBufRP) {
			KeyBuf[KeyBufWP] = code;
			KeyBufWP = newwp;
			//printf("KeyBufWP: %d\n", KeyBufWP);
		}
	}
#else
	send_keycode(code, P6K_DOWN);
#endif

	printf("JoyKeyState: 0x%x\n", JoyKeyState);

	switch (wp) {
	case SDLK_UP:
		//puts("key up");
		if (!(JoyKeyState&JOY_DOWN))
			JoyKeyState |= JOY_UP;
		break;

	case SDLK_DOWN:
		if (!(JoyKeyState&JOY_UP))
			JoyKeyState |= JOY_DOWN;
		break;

	case SDLK_LEFT:
		if (!(JoyKeyState&JOY_RIGHT))
			JoyKeyState |= JOY_LEFT;
		break;

	case SDLK_RIGHT:
		if (!(JoyKeyState&JOY_LEFT))
			JoyKeyState |= JOY_RIGHT;
		break;

	case SDLK_z:
		//puts("key z");
		if (Config.JoyKeyReverse)
			JoyKeyState |= JOY_TRG2;
		else
			JoyKeyState |= JOY_TRG1;
		break;

	case SDLK_x:
		//puts("key x");
		if (Config.JoyKeyReverse)
			JoyKeyState |= JOY_TRG1;
		else
			JoyKeyState |= JOY_TRG2;
		break;
	}
}

// ----------------------------------
//	WM_KEYUP
// ----------------------------------
void
Keyboard_KeyUp(uint32_t wp)
{
	int32_t  code;
	uint8_t newwp;
#if 0
	if (wp & ~0xff) {
		if (wp == GDK_VoidSymbol)
			code = NC;
		else if ((wp & 0xff00) == 0xff00)
			code = KeyTable[(wp & 0xff) | 0x100];
		else
			code = NC;
	} else
		code = KeyTable[wp & 0xff];
#endif
	code = get_x68k_keycode(wp);
	if (code < 0) {
		return;
	}
	if(code == 0x70){ shiftdown = 0; }

#if 0
	if (code != NC) {
		newwp = ((KeyBufWP + 1) & (KeyBufSize - 1));
		if (newwp != KeyBufRP) {
			KeyBuf[KeyBufWP] = code | 0x80;
			KeyBufWP = newwp;
		}
	}
#else
	send_keycode(code, P6K_UP);
#endif

	//printf("JoyKeyState: 0x%x\n", JoyKeyState);

	switch(wp) {
	case SDLK_UP:
		JoyKeyState &= ~JOY_UP;
		break;

	case SDLK_DOWN:
		JoyKeyState &= ~JOY_DOWN;
		break;

	case SDLK_LEFT:
		JoyKeyState &= ~JOY_LEFT;
		break;

	case SDLK_RIGHT:
		JoyKeyState &= ~JOY_RIGHT;
		break;

	case SDLK_z:
		if (Config.JoyKeyReverse)
			JoyKeyState &= ~JOY_TRG2;
		else
			JoyKeyState &= ~JOY_TRG1;
		break;

	case SDLK_x:
		if (Config.JoyKeyReverse)
			JoyKeyState &= ~JOY_TRG1;
		else
			JoyKeyState &= ~JOY_TRG2;
		break;
	}

}

// ----------------------------------
//	Key Check
//	1フレーム中に4回（2400bps/10bit/60fpsとすれば、だが）呼ばれて、MFPにデータを送る
// ----------------------------------

void
Keyboard_Int(void)
{
	if (KeyBufRP != KeyBufWP) {
		//p6logd("KeyBufRP:%d, KeyBufWP:%d\n", KeyBufRP, KeyBufWP);
		if (!KeyIntFlag) {
			LastKey = KeyBuf[KeyBufRP];
			KeyBufRP = ((KeyBufRP+1)&(KeyBufSize-1));
			KeyIntFlag = 1;
			MFP_Int(3);
		}
	} else if (!KeyIntFlag) {
		LastKey = 0;
	}
}

/********** ソフトウェアキーボード **********/

#if defined(USE_OGLES11)

// 選択しているキーの座標 (初期値'Q')
int32_t  kbd_kx = 1, kbd_ky = 2;

#define KEYXMAX 32767
int32_t Keyboard_get_key_ptr(int32_t x, int32_t y)
{
	int32_t i, j;
	int32_t p = 0;
	int32_t tx, ty; // tmp x, tmp y

	tx = KEYXMAX;
	ty = 0;
	i = 0;
	while (kbd_key[i].x != -1) {
		// skip dummy
		if (kbd_key[i].c == 0) {
			i++;
			continue;
		}
		if (tx > kbd_key[i].x) {
			if (ty == y)
				break;
			ty++;
		}
		tx = kbd_key[i++].x;
	}

	return i + x;
}

static void set_ptr_to_kxy(int32_t p)
{
	int32_t kx = 0, ky = 0;
	int32_t i;

	for (i = 0; i < p; i++) {
		if (kbd_key[i].x > kbd_key[i + 1].x) {
			ky++;
			kx = 0;
		} else {
			kx++;
		}
	}

	kbd_kx = kx, kbd_ky = ky;
}

static void set_near_key(int32_t p, int32_t tx)
{
	while (kbd_key[p].x < kbd_key[p + 1].x) {
		if (kbd_key[p].x >= tx) {
			// x座標の一番近いキーに移動する
			if (abs(kbd_key[p].x - tx)
			    > abs(tx - kbd_key[p - 1].x)) {
				p--;
			}
			break;
		}
		p++;
	}
	set_ptr_to_kxy(p);
}

static void key_up(void)
{
	int32_t p;
	int32_t c = 2;
	int32_t tx; // tmp x

	p = Keyboard_get_key_ptr(kbd_kx, kbd_ky);
	tx = kbd_key[p].x;

	// 上の行の先頭を探す
	while (p > 0) {
		if (kbd_key[p - 1].x > kbd_key[p].x) {
			c--;
			if (c == 0) {
				break;
			}
		}
		p--;
	}

	if (kbd_key[p].x == tx) {
		set_ptr_to_kxy(p);
		return;
	}

	set_near_key(p, tx);
}

static void key_down(void)
{
	int32_t p;
	int32_t tx; // tmp x

	p = Keyboard_get_key_ptr(kbd_kx, kbd_ky);
	tx = kbd_key[p].x;

	// 下の行の先頭を探す
	while (kbd_key[p].x != -1) {
		if (kbd_key[p + 1].x < kbd_key[p].x) {
			p++;
			break;
		}
		p++;
	}

	set_near_key(p, tx);
}


// dx と dy はどちらか一方は 0 とする。 dx: -1/0/+1 dy: -1/0/+1
static void mv_key(int32_t dx, int32_t dy)
{
	int32_t p;

	if ((kbd_ky == 0 && dy == -1) || (kbd_ky == 5 && dy == +1)) {
		return;
	}

	if (kbd_kx == 0 && dx == -1) {
		return;
	}
	p = Keyboard_get_key_ptr(kbd_kx, kbd_ky);
	if (dx == +1 &&
	    kbd_key[p + 1].x != -1 && kbd_key[p].x > kbd_key[p + 1].x) {
		return;
	}

	WinDraw_reverse_key(kbd_kx, kbd_ky);

	// 飛ばし先の微調整が必要なものたち
	// カーソルキー周りは面倒なので微調整なし
	if (kbd_kx == 12 && kbd_ky == 3 && dx == +1) {
		// ] の右となりは RET に飛ばす
		kbd_kx = 14, kbd_ky = 2;
	} else if (kbd_kx == 12 && kbd_ky == 5 && dx == +1) {
		// テンキーの . の右となりは ENT に飛ばす
		kbd_kx = 19, kbd_ky = 4;
	} else {
		// 微調整の必要なし
		kbd_kx += dx;
	}

	if (dy < 0) {
		key_up();
	}
	if (dy > 0) {
		key_down();
	}

	// RETの左側のダミーキーはRETに移動する
	if (kbd_kx == 13 && kbd_ky == 2) {
		if (dx == 0) {
			// 上下からダミーキーに行った場合
			kbd_kx = 14;
		} else {
			// 左右からダミーキーに行った場合
			kbd_kx += dx;
		}
	}

	WinDraw_reverse_key(kbd_kx, kbd_ky);
}

static void send_key(int32_t flag)
{

	uint8_t code;

	code = kbd_key[Keyboard_get_key_ptr(kbd_kx, kbd_ky)].c;
	//p6logd("sendkey: %d", code);

	send_keycode(code, flag);
}

void Keyboard_skbd(void)
{
	static kx = 1, ky = 2;
	uint8_t joy;

	joy = get_joy_downstate();
	reset_joy_downstate();

	if (!(joy & JOY_UP)) {
		mv_key(0, -1);
	}
	if (!(joy & JOY_DOWN)) {
		mv_key(0, +1);
	}
	if (!(joy & JOY_LEFT)) {
		mv_key(-1, 0);
	}
	if (!(joy & JOY_RIGHT)) {
		mv_key(+1, 0);
	}
	if (!(joy & JOY_TRG1)) {
		send_key(P6K_DOWN);
	}
	if (!(joy & JOY_TRG2)) {
		// BSキーをkey down
		send_keycode(0xf, P6K_DOWN);
	}

	joy = get_joy_upstate();
	reset_joy_upstate();
	if (joy & JOY_TRG1) {
		send_key(P6K_UP);
	}
	if (joy & JOY_TRG2) {
		// BSキーをkey up
		send_keycode(0xf, P6K_UP);
	}

}

int32_t skbd_mode = FALSE;

void Keyboard_ToggleSkbd(void)
{
	skbd_mode = (skbd_mode == TRUE)? FALSE : TRUE;
}

#endif //(USE_OGLES11)

int32_t Keyboard_IsSwKeyboard(void)
{
#if defined(USE_OGLES11)
	if (kbd_x < 700) {
		return TRUE;
	} else {
		return FALSE;
	}
#else
	return FALSE;
#endif
}
