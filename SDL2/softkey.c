/* 
 * Copyright (c) 2003 NONAKA Kimihiro
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
/*
#include <unistd.h>
#include "common.h"
#include "SDL2/SDL.h"

#include "keyboard.h"
#include "winx68k.h"

extern int32_t  winx,winy;
extern SDL_Surface *menu_surface;

extern void set_mfs(int32_t fs);
extern void set_mbcolor(uint16_t c);
extern void set_sbp(uint16_t *p);
extern void set_mlocate(int32_t x, int32_t y);
extern void draw_str(char *cp, uint32_t flg);
extern void set_mcolor(uint16_t c);
*/

SDL_Window *sft_kbd_window;
SDL_Surface *sftkey_draw;
SDL_Surface *keydraw_buffer;


/********** ソフトウェアキーボード描画 **********/

#define KBDBUF_WIDTH 766
#define KBDBUF_HIGHT 218
#define KBD_FS 16 // keyboard font size : 16

// 選択しているキーの座標 (初期値'Q')
int32_t  kbd_kx = 1, kbd_ky = 2;

// キーを反転する
void softkey_reverse(int32_t x, int32_t y)
{
	uint16_t *p;
	int32_t kp;
	int32_t i, j;
	int32_t Bpp = keydraw_buffer->format->BytesPerPixel;

	//kp = Keyboard_get_key_ptr(kbd_kx, kbd_ky);

	p = keydraw_buffer->pixels + (keydraw_buffer->w * Bpp * kbd_key[kp].y) + (kbd_key[kp].x * Bpp);

	for (i = 0; i < kbd_key[kp].h; i++) {
		for (j = 0; j < kbd_key[kp].w; j++) {
			*p = ~(*p);
			p++;
		}
		p = p + KBDBUF_WIDTH - kbd_key[kp].w;
	}
}

void Soft_kbd_CleanupScreen(void)
{
	if(sft_kbd_window != NULL){
	 SDL_DestroyWindow(sft_kbd_window);
	 sft_kbd_window = NULL;
	}
}

void draw_soft_kbd(uint32_t ms_x,uint32_t ms_y)
{
	int32_t i, x, y,Bpp;
	uint16_t *p;
	uint16_t keycolor;

	// Open KeyBoard Window
	if(sft_kbd_window == NULL){
		sft_kbd_window = SDL_CreateWindow("X68000 keyboard", winx, winy, KBDBUF_WIDTH, KBDBUF_HIGHT, SDL_WINDOW_SHOWN);
		sftkey_draw = SDL_GetWindowSurface(sft_kbd_window); /*描画用*/
		keydraw_buffer = SDL_CreateRGBSurface(0, 800, 220, 16, 0xf800, 0x07e0, 0x001f, 0);
	}

	Bpp = keydraw_buffer->format->BytesPerPixel;

	set_sbp(keydraw_buffer->pixels);
	set_mbcolor(0);
	set_mcolor(0);
	set_mfs(KBD_FS);

	// キーボードの背景
	p = keydraw_buffer->pixels;
	for (y = 0; y < keydraw_buffer->h; y++) {
		for (x = 0; x < keydraw_buffer->w; x++) {
			*p++ = (0x7800 | 0x03e0 | 0x000f);
		}
	}

	// キーの描画
	for (i = 0; kbd_key[i].x != -1; i++) {
		if((kbd_key[i].x < ms_x) && (kbd_key[i].y < ms_y) && 
			(kbd_key[i].x+kbd_key[i].w > ms_x) && (kbd_key[i].y+kbd_key[i].h > ms_y)){
			send_keycode((kbd_key[i].c & 0x7f), 2);// send keydown event
			keycolor = 0x3333;
			send_keycode((kbd_key[i].c & 0x7f), 1);// send keyup event
		}
		else{
			keycolor = 0xffff;
		}

		p = keydraw_buffer->pixels + (kbd_key[i].y * keydraw_buffer->w * Bpp) + (kbd_key[i].x * Bpp);
		for (y = 0; y < kbd_key[i].h; y++) {
			for (x = 0; x < kbd_key[i].w; x++) {
				if (x >= (kbd_key[i].w - 1)
				    || y >= (kbd_key[i].h - 1)) {
					// キーに影をつけ立体的に見せる
					*p++ = 0x0000;
				} else {
					*p++ = keycolor;
				}
			}
			p = p + keydraw_buffer->w - kbd_key[i].w;
		}
		if (strlen(kbd_key[i].s) == 3 && *(kbd_key[i].s) == 'F') {
			// FUNCキー刻印描画
			set_mlocate(kbd_key[i].x + kbd_key[i].w / 2
				    - strlen(kbd_key[i].s) * (8 / 2)
				    + (strlen(kbd_key[i].s) - 1) * 3 / 2,
				    kbd_key[i].y + kbd_key[i].h / 2 - 8 / 2);
			set_mfs(8);
			draw_str(kbd_key[i].s,1);
			set_mfs(KBD_FS);
		} else {
			// 刻印は上下左右ともセンタリングする
			set_mlocate(kbd_key[i].x + kbd_key[i].w / 2
				    - strlen(kbd_key[i].s) * (KBD_FS / 2 / 2),
				    kbd_key[i].y
				    + kbd_key[i].h / 2 - KBD_FS / 2);
			draw_str(kbd_key[i].s,1);
		}
	}

	//softkey_reverse(kbd_kx, kbd_ky);

	SDL_BlitSurface(keydraw_buffer, NULL, sftkey_draw, NULL);
	SDL_UpdateWindowSurface(sft_kbd_window);

	set_mfs(24); // 元に戻す
	set_sbp((uint16_t *)(menu_surface->pixels));

}

