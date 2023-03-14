/* 
Software Keyboard for px68k by SDL2
use keymap-table:keytbl.inc
*/

SDL_Window *sft_kbd_window;
SDL_Surface *sftkey_draw;
SDL_Surface *keydraw_buffer;

uint8_t Key_X68, LED_X68;

/******* ソフトウェアキーボード描画 *******/

#define softkey_width 766
#define softkey_hight 218

#define softkey_fontsize 16 // keyboard font size:16

void Soft_kbd_CleanupScreen(void)
{
	if(sft_kbd_window != NULL){
	 SDL_DestroyWindow(sft_kbd_window);
	 SDL_FreeSurface(keydraw_buffer);
	 sft_kbd_window = NULL;
	}
}

void draw_soft_kbd(uint32_t ms_x,uint32_t ms_y, uint8_t keyboardLED)
{
	int32_t i, x, y,Bpp;
	uint16_t *p;
	uint16_t keycolor;
	SDL_Rect keyrect;

	// Window KeyBoard Panel
	if(sft_kbd_window == NULL){
		sft_kbd_window = SDL_CreateWindow("X68000 keyboard", winx, winy, softkey_width, softkey_hight, SDL_WINDOW_SHOWN);
		sftkey_draw = SDL_GetWindowSurface(sft_kbd_window); /*描画用*/
		keydraw_buffer = SDL_CreateRGBSurface(0, 800, 220, 16, 0xf800, 0x07e0, 0x001f, 0);
		LED_X68 = keyLED;
	}

	Bpp = keydraw_buffer->format->BytesPerPixel;

	// LED変化なしの場合はret
	if((ms_x == 0)&&(ms_y == 0)){
		if(!(keyboardLED & 0x80) && (keyboardLED != 0)) return;
		if((keyboardLED & 0x7f) == (LED_X68 & 0x7f)) return;
	}

	set_sbp(keydraw_buffer->pixels);
	set_mbcolor(0);
	set_mcolor(0);
	set_mfs(softkey_fontsize);

	// キーボードの背景(全画面塗りつぶし)
	SDL_FillRect(keydraw_buffer, NULL, (0x7800 | 0x03e0 | 0x000f));

	// 全キーの描画
	for (i = 0; kbd_key[i].x != -1; i++) {

	  if((ms_x == 0)&&(ms_y == 0)){// key更新なし(LEDのみ)
		if(Key_X68 == kbd_key[i].c) keycolor = 0x3333;
		else keycolor = 0xffff;
		LED_X68 = keyboardLED;
	  }
	  else{//key押下判定
		// KEY match
		if((kbd_key[i].x < ms_x) && (kbd_key[i].y < ms_y) && 
			(kbd_key[i].x+kbd_key[i].w > ms_x) && (kbd_key[i].y+kbd_key[i].h > ms_y)){
			keycolor = 0x3333;
		}
		else{
			keycolor = 0xffff;
		}

		// KEY eventをX68000に送る
		if((Key_X68 != kbd_key[i].c) && (keycolor == 0x3333)){
		  send_keycode((kbd_key[i].c & 0x7f), 2);// send keydown event
		  Key_X68 = kbd_key[i].c;
		}
		if((Key_X68 == kbd_key[i].c) && (keycolor == 0xffff)){
		  send_keycode((kbd_key[i].c & 0x7f), 1);// send keyup event
		  Key_X68 = 0;
		}
	  }

		// Key Rect Drawing
		p = keydraw_buffer->pixels + (kbd_key[i].y * keydraw_buffer->w * Bpp) + (kbd_key[i].x * Bpp);
		keyrect.x=kbd_key[i].x; keyrect.y=kbd_key[i].y;
		keyrect.w=kbd_key[i].w; keyrect.h=kbd_key[i].h;
		SDL_FillRect(keydraw_buffer, &keyrect, 0x0000);//shadow
		keyrect.w -=2; keyrect.h -=2;
		SDL_FillRect(keydraw_buffer, &keyrect, keycolor);

		// KeyTop Charactor
		x=kbd_key[i].x + (kbd_key[i].w-softkey_fontsize)/2 - ((strlen(kbd_key[i].s)-1)*2);
		y=kbd_key[i].y + (kbd_key[i].h-softkey_fontsize)/2 - 1;
		set_mlocate(x,y);
		draw_str(kbd_key[i].s,1);
	}

	// LED drawing
	keyrect.w=16; keyrect.h=6;//共通
	if(!(LED_X68 & 0x01)){//かな
		keyrect.x=kbd_key[12].x +8; keyrect.y=kbd_key[12].y +24;
		SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	}
	if(!(LED_X68 & 0x02)){//ローマ字
		keyrect.x=kbd_key[13].x +8; keyrect.y=kbd_key[13].y +24;
		SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	}
	if(!(LED_X68 & 0x04)){//コード入力
		keyrect.x=kbd_key[14].x +8; keyrect.y=kbd_key[14].y +24;
		SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	}
	if(!(LED_X68 & 0x08)){//CAPSLOCK
		keyrect.x=kbd_key[15].x +8; keyrect.y=kbd_key[15].y +24;
		SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	}
	if(!(LED_X68 & 0x10)){//INS
		keyrect.x=kbd_key[35].x +8; keyrect.y=kbd_key[35].y +24;
		SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	}
	if(!(LED_X68 & 0x20)){//ひらがな
		keyrect.x=kbd_key[101].x +8; keyrect.y=kbd_key[101].y +24;
		SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	}
	if(!(LED_X68 & 0x40)){//全角
		keyrect.x=kbd_key[108].x +8; keyrect.y=kbd_key[108].y +24;
		SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	}

	// 描画結果を画面に表示
	SDL_BlitSurface(keydraw_buffer, NULL, sftkey_draw, NULL);
	SDL_UpdateWindowSurface(sft_kbd_window);

	 // 元に戻す
	set_mfs(24);
	set_sbp((uint16_t *)(menu_surface->pixels));

  return;
}

