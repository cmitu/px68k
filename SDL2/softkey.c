/* 
Software Keyboard for px68k by SDL2
use keymap-table:keytbl.inc
*/

SDL_Window   *sft_kbd_window;
SDL_Renderer *sft_kbd_render;
SDL_Texture  *sft_kbd_texture;
SDL_Surface  *keydraw_buffer;

uint8_t Key_X68, LED_X68;

/******* ソフトウェアキーボード描画 *******/

#define softkey_width 766
#define softkey_hight 218

#define softkey_fontsize 16 // keyboard font size:16

/*非表示で初期化*/
void Soft_kbd_CreateScreen(void)
{
	sft_kbd_window  = SDL_CreateWindow("X68000 keyboard", winx+16, winy+380, softkey_width, softkey_hight, SDL_WINDOW_HIDDEN);
	sft_kbd_render  = SDL_CreateRenderer(sft_kbd_window, -1, SDL_RENDERER_ACCELERATED);
	sft_kbd_texture = SDL_CreateTexture(sft_kbd_render, SDL_PIXELFORMAT_RGB565,
							SDL_TEXTUREACCESS_STREAMING, softkey_width, softkey_hight);
	SDL_SetRenderTarget(sft_kbd_render, sft_kbd_texture);

	keydraw_buffer = SDL_CreateRGBSurface(0, 800, 220, 16, 0xf800, 0x07e0, 0x001f, 0);//描画用

	if (sft_kbd_window == NULL) {
		p6logd("Soft_Keyboard Window: %ld", sft_kbd_window);
	}

 return;
}

/*終了時*/
void Soft_kbd_CleanupScreen(void)
{
	SDL_FreeSurface(keydraw_buffer);
	SDL_DestroyTexture(sft_kbd_texture);
	SDL_DestroyRenderer(sft_kbd_render);
	SDL_DestroyWindow(sft_kbd_window);

  return;
}

/*表示制御*/
void Soft_kbd_Show(uint32_t flg)
{
extern void draw_soft_kbd(uint32_t ms_x,uint32_t ms_y, uint8_t keyboardLED);
  if(flg == 0) SDL_HideWindow(sft_kbd_window);
  if(flg == 1){
	SDL_ShowWindow(sft_kbd_window);
	draw_soft_kbd(0,0,keyLED);//Redraw
  }
  if(flg == 2){
	SDL_SetWindowPosition(sft_kbd_window, winx+16, winy+430);
  }

  return;
}

/*描画する*/
void draw_soft_kbd(uint32_t ms_x,uint32_t ms_y, uint8_t keyboardLED)
{
	int32_t i, x, y,Bpp;
	uint16_t *p;
	uint16_t keycolor;
	SDL_Rect keyrect;

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
		keyrect.x=kbd_key[i].x; keyrect.y=kbd_key[i].y;
		keyrect.w=kbd_key[i].w; keyrect.h=kbd_key[i].h;
		SDL_FillRect(keydraw_buffer, &keyrect, 0x0000);//shadow
		keyrect.w -=2; keyrect.h -=2;
		SDL_FillRect(keydraw_buffer, &keyrect, keycolor);

		// KeyTop Charactor
		x=kbd_key[i].x + (kbd_key[i].w-softkey_fontsize)/2 - ((strlen(kbd_key[i].s)-1)*2);
		y=kbd_key[i].y + (kbd_key[i].h-softkey_fontsize)/2 - 1;
		if(strlen(kbd_key[i].s2) == 0){
		 set_mlocate(x,y);
		 draw_str(kbd_key[i].s,1);
		}
		else{
		 set_mlocate(x-3,y-2);
		 draw_str(kbd_key[i].s,1);
		 set_mlocate(x+5,y+5);
		 draw_str(kbd_key[i].s2,1);
		}
	}

	// LED drawing
	keyrect.w=16; keyrect.h=6;//共通
	uint32_t KeyLED[] = {12,13,14,15,35,101,108};
	y = LED_X68;
	for(i=0; i<7; i++){
	 if(!(y & 0x01)){//かな
	   keyrect.x=kbd_key[KeyLED[i]].x +8; keyrect.y=kbd_key[KeyLED[i]].y +24;
	   SDL_FillRect(keydraw_buffer, &keyrect, (0x0000 | 0x03e0 | 0x0000));
	 }
	 y >>= 1;
	}

	// 描画結果を画面に表示(SDL2)
	SDL_UpdateTexture(sft_kbd_texture, NULL,keydraw_buffer->pixels, 800*Bpp );
	SDL_RenderCopy(sft_kbd_render, sft_kbd_texture, NULL, NULL);
	SDL_RenderPresent(sft_kbd_render);

	 // 元に戻す
	set_mfs(24);
	set_sbp((uint16_t *)(menu_surface->pixels));

  return;
}

