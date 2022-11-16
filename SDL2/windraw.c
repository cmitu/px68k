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
#include <unistd.h>
#include "common.h"
#include "SDL2/SDL.h"

#ifdef USE_OGLES11
#include "SDL2/SDL_opengles.h"
#endif

#include "winx68k.h"
#include "winui.h"

#include "bg.h"
#include "crtc.h"
#include "gvram.h"
#include "mouse.h"
#include "palette.h"
#include "prop.h"
#include "status.h"
#include "tvram.h"
#include "joystick.h"
#include "keyboard.h"
#include "x68kmemory.h"

#if 0
#include "../icons/keropi.xpm"
#endif

extern int32_t Debug_Text, Debug_Grp, Debug_Sp;

uint16_t *ScrBuf = 0;

#if defined(USE_OGLES11)
uint16_t *menu_buffer;
uint16_t *kbd_buffer;
#endif

int32_t JoyDirection;
int32_t Draw_Opaque;
int32_t FullScreenFlag = 0;
extern uint8_t Draw_RedrawAllFlag;
uint8_t Draw_DrawFlag = 1;
uint8_t Draw_ClrMenu = 0;


int32_t  winx = 0, winy = 0; /*window position*/
uint32_t winh = 0, winw = 0; /*window width hight*/
uint32_t root_width, root_height; /*primary screen size*/
uint32_t FrameCount = 0;
int32_t  SplashFlag = 0;
int32_t  ScreenClearFlg = 0;

uint16_t WinDraw_Pal16B, WinDraw_Pal16R, WinDraw_Pal16G;

uint32_t WindowX = 0;
uint32_t WindowY = 0;

#ifdef USE_OGLES11
static GLuint texid[11];
#endif

/* Drawing SDL2 buffer */
extern SDL_Window *sdl_window;
extern SDL_Renderer *sdl_render;
extern SDL_Texture *sdl_texture;
extern SDL_Surface *sdl_x68screen;

#if !defined(USE_OGLES11)
SDL_Surface *menu_surface;
#endif

extern int32_t VID_MODE, CHANGEAV_TIMING;


int32_t WinDraw_Init(uint32_t err_msg_no);
void WinDraw_StartupScreen(void);
void WinDraw_Draw(void);
int32_t WinDraw_ChangeSize(void);
uint32_t conv_utf8tosjis();

#define	APPNAME	"Keropi"

static int32_t drawW=0,drawH=0; /*start X ,Y*/
static uint32_t surfaceW=800,surfaceH=600; /*width Hight*/
static uint32_t HLINE_TOTAL_1,CRTC_HSTART_1,CRTC_HEND_1;/*Store*/
static uint32_t VLINE_TOTAL_1,CRTC_VSTART_1,CRTC_VEND_1,CRTC_VStep_1;

/*UTF8 conv Table and util. by Kameya*/
#include "kanjiconv.c"

/* X68 change screen size (Call from CRTC) */
/* Clear -> ChangeScreen -> Clear  */
int32_t WinDraw_ChangeSize(void)
{
	ScreenClearFlg=1;
	return TRUE;
}
/* X68 change screen size (Delayed Change) */
/* jadgment change or not.  */
int32_t WinDraw_ChangeSizeGO(void)
{

	if((HLINE_TOTAL<100) || (HLINE_TOTAL>1200)) return FALSE; 	/*H-DrawArea check*/
	if((VLINE_TOTAL<100) || (VLINE_TOTAL>700)) return FALSE; 	/*V-DrawArea check*/

	/* No change check */
	if((HLINE_TOTAL==HLINE_TOTAL_1) && (CRTC_HSTART==CRTC_HSTART_1) && (CRTC_HEND==CRTC_HEND_1) &&
		(VLINE_TOTAL==VLINE_TOTAL_1) && (CRTC_VSTART==CRTC_VSTART_1) && (CRTC_VEND==CRTC_VEND_1)&&(CRTC_VStep==CRTC_VStep_1)){
	return FALSE;
	}

	int32_t VLINE_TOTAL2,VSTART2,HSTART2=CRTC_HSTART*8;

	//垂直解像度補正(2Line描画)
	switch(CRTC_VStep){
		case 1:		VLINE_TOTAL2 = VLINE_TOTAL/2; VSTART2 = CRTC_VSTART / 2;
		  break;
		case 4:		VLINE_TOTAL2 = VLINE_TOTAL*2; VSTART2 = CRTC_VSTART * 2;
		  break;
		default:	VLINE_TOTAL2 = VLINE_TOTAL;  VSTART2 = CRTC_VSTART;
		  break;
	}

	// Screen Size は総数から非表示部分を取り除いて求める(TextDotX/Yは実際のCRTの表示範囲ではない)
	uint32_t ScreenX = HLINE_TOTAL  * 0.69; ScreenX /=32; ScreenX++; ScreenX *=32;
	uint32_t ScreenY = VLINE_TOTAL2 * 0.90; ScreenY /=32; ScreenY++; ScreenY *=32;
	
	// 表示範囲がオーバースキャンの場合
	if(ScreenX<TextDotX){ScreenX=TextDotX;HSTART2=0;}
	if(ScreenY<TextDotY){ScreenY=TextDotY;VSTART2=0;}

	//printf("TOTAL %d:%d ScrenXY %d:%d TdotXY %d:%d StartXY %d:%d\n",HLINE_TOTAL,VLINE_TOTAL2,ScreenX,ScreenY,
	//	TextDotX,TextDotY,HSTART2, VSTART2);

	/*=== Do Change X68000 Screen Size ===*/
	WinDraw_InitWindowSize(ScreenX, ScreenY, HSTART2, VSTART2);

	// 保存
	HLINE_TOTAL_1=HLINE_TOTAL;
	CRTC_HSTART_1=CRTC_HSTART;
	CRTC_HEND_1=CRTC_HEND;
	VLINE_TOTAL_1=VLINE_TOTAL;
	CRTC_VSTART_1=CRTC_VSTART;
	CRTC_VEND_1=CRTC_VEND;
	CRTC_VStep_1=CRTC_VStep;


	return TRUE;
}

/*Do Change Screen Size!*/
void
WinDraw_InitWindowSize(uint32_t ScreenX, uint32_t ScreenY, uint32_t StartX, uint32_t StartY)
{

	SDL_RenderClear(sdl_render);/*screen buffer clear*/

	switch(ScreenX){
	  case 513 ... 900: //====768x512/256   640x480/400/240/200====
		 switch(ScreenY){
		 case 257 ... 511:
		   surfaceW=ScreenX; drawW=StartX-224;
		   surfaceH=ScreenY; drawH=StartY-40;
		  break;
		 case 000 ... 256:
		   surfaceW=ScreenX; drawW=StartX-224;
		   surfaceH=ScreenY; drawH=StartY-20;
		  break;
		 }
		if(TextDotX==768){ surfaceW=800; drawW=((800-TextDotX)/2);}/*H-Dot2Dot*/
		if(TextDotY==512){ surfaceH=600; drawH=((600-TextDotY)/2);}/*V-Dot2Dot*/
	   break;
	  case 420 ... 512: //====512x512/256====
	   surfaceW=ScreenX; surfaceH=ScreenY; drawW=StartX-136;
		switch(ScreenY){
		 case 257 ... 600:
		   drawH=StartY-40;
		  break;
		 case 000 ... 256:
		   drawH=StartY-20;
		  break;
		}
	   break;
	  case 257 ... 419: //====416x256 384x256====
	   surfaceW=ScreenX; surfaceH=ScreenY; drawW=StartX-88;
		switch(ScreenY){
		 case 257 ... 600:
		   drawH=StartY-40;
		  break;
		 case 000 ... 256:
		   drawH=StartY-20;
		  break;
		}
	   break;
	  case 000 ... 256: //====256x256以下====
	   surfaceW=256; surfaceH=256; drawW=StartX-48;
		switch(ScreenY){
		 case 257 ... 600:
		   drawH=StartY-40;
		  break;
		 case 000 ... 256:
		   drawH=StartY-20;
		   break;
		}
	   break;
	}

	//printf("XY %d:%d %d:%d\n",surfaceW,surfaceH,drawW,drawH);

	//clippng...
	if(drawW<0){ drawW=0; }
	if(drawH<0){ drawH=0; }

	//nealy full screen...
	if((surfaceW - TextDotX)<30){surfaceW=TextDotX;drawW=0;}
	if((surfaceH - TextDotY)<16){surfaceH=TextDotY;drawH=0;}

	return;
}

/*プライマリ・Diplayの大きさをGET*/
void WinGetRootSize(void)
{
    int32_t display_index;
	SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

	/*==マルチ・ディスプレイ　プライマリ(0)の現在の解像度を調査==*/
	display_index = 0;

        if (SDL_GetCurrentDisplayMode(display_index, &mode) == 0)
        {
         root_width      = mode.w;/*現在の解像度*/
         root_height     = mode.h;
         //sdl_byte_per_pixel = SDL_BYTESPERPIXEL(mode.format); /*bit深度*/
        }
        else
		{
		 root_width = 1280; //ダミー
		 root_height = 800; //ダミー
		}

return;
}


//static int dispflag = 0;
void WinDraw_StartupScreen(void)
{
	/*clear screen:一度全画面描画更新しておく*/
	ScreenClearFlg=1;
	return;
}

void WinDraw_CleanupScreen(void)
{
}

/* Window Size set 
  0: 800x600(default) or Resizable
  1: full-screen  */
void WinDraw_ChangeMode(int32_t flg)
{
	uint32_t w_flags = 0;

	if (flg == 1) {
	  w_flags =  SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;
	  SDL_ShowCursor(SDL_DISABLE);
	}
	else{
	  w_flags =  SDL_WINDOW_SHOWN;
	  SDL_ShowCursor(SDL_ENABLE);
	}

	if (Config.WinStrech == 1){w_flags |= SDL_WINDOW_RESIZABLE;}

	/* clear screen */
	SDL_RenderClear(sdl_render);

	/*texture 描画のみ有効。（surface描画は画面真っ黒）*/
	SDL_SetWindowFullscreen(sdl_window, w_flags);

	/* clear screen */
	ScreenClearFlg=1;

 return ;
}

void WinDraw_ShowSplash(void)
{
}

void WinDraw_HideSplash(void)
{
}

static void draw_kbd_to_tex(void);

int32_t WinDraw_Init(uint32_t err_msg_no)
{
	int32_t i, j, k;

	WindowX = 768;
	WindowY = 512;

	/* SDL2 create Texture */
	sdl_texture = SDL_CreateTexture(sdl_render,SDL_PIXELFORMAT_RGB565,
		SDL_TEXTUREACCESS_STREAMING, 800, 600);
	if (sdl_texture == NULL) {
		fprintf(stderr, "can't create texture.\n");
		return FALSE;
	}

	// SDL2 Color Table
	WinDraw_Pal16R = 0xf800;
	WinDraw_Pal16G = 0x07e0;
	WinDraw_Pal16B = 0x001f;
	//printf("R: %x, G: %x, B: %x\n", WinDraw_Pal16R, WinDraw_Pal16G, WinDraw_Pal16B);


#if defined(USE_OGLES11)
	ScrBuf = malloc(1024*1024*2+2000); // OpenGL ES 1.1 needs 2^x pixels
	if (ScrBuf == NULL) {
		return FALSE;
	}

	kbd_buffer = malloc(1024*1024*2+2000); // OpenGL ES 1.1 needs 2^x pixels
	if (kbd_buffer == NULL) {
		return FALSE;
	}

	p6logd("kbd_buffer 0x%x", kbd_buffer);

	memset(texid, 0, sizeof(texid));
	glGenTextures(11, &texid[0]);

	// texid[0] for the main screen
	glBindTexture(GL_TEXTURE_2D, texid[0]);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	uint16_t BtnTex[32*32];
	//とりあえず薄めの緑で。
	for (i = 0; i < 32*32; i++) {
		BtnTex[i] = 0x03e0;
	}

	// ボタン用テクスチャ。とりあえず全部同じ色。
	for (i = 1; i < 9; i++) {
		glBindTexture(GL_TEXTURE_2D, texid[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		if (i == 7) {
			// とりあえずキーボードonボタンは薄めの黄色で。
			for (j = 0; j < 32*32; j++) {
				BtnTex[j] = (0x7800 | 0x03e0);
			}
		}
		if (i == 8) {
			// とりあえずmenu onボタンは薄めの白色で。
			for (j = 0; j < 32*32; j++) {
				BtnTex[j] = (0x7800 | 0x03e0 | 0x0f);
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, BtnTex);
	}

	// メニュー描画用テクスチャ。
	glBindTexture(GL_TEXTURE_2D, texid[9]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, menu_buffer);

	draw_kbd_to_tex();

	// ソフトウェアキーボード描画用テクスチャ。
	glBindTexture(GL_TEXTURE_2D, texid[10]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, kbd_buffer);

#else // OpenGL ES end

	/* X68000 Drawing Area for SDL2 */
	sdl_x68screen = SDL_CreateRGBSurface(0, 800, 600, 16, WinDraw_Pal16R, WinDraw_Pal16G, WinDraw_Pal16B, 0);
	if (sdl_x68screen == NULL) return FALSE;
	ScrBuf = sdl_x68screen->pixels;

#endif

// --- Draw Init Err Message ---
#include "msg_tif.h"
	uint16_t *p;
	uint8_t dt;
	if(err_msg_no != 0){
	  for(j=0; j<14; j++){
	   for(i=0; i<38; i++){ // 304/8=38byte
	    p=ScrBuf + 800*(280+j)+230+(i*8);
	    dt = no_rom_msg[j*38+i];
		for(k=0; k<8; k++){
	    if((dt & 0x80)!=0){*p = 0xffff;} //color depth = 16bit
		dt = dt << 1;
		p++;
		}
	   }
	  }
	  TextDotX=800; TextDotY=600;
	  WinDraw_Draw();
	}

	return TRUE;
}

void
WinDraw_Cleanup(void)
{
}

void
WinDraw_Redraw(void)
{

	TVRAM_SetAllDirty();
}

#ifdef USE_OGLES11
#define SET_GLFLOATS(dst, a, b, c, d, e, f, g, h)		\
{								\
	dst[0] = (a); dst[1] = (b); dst[2] = (c); dst[3] = (d);	\
	dst[4] = (e); dst[5] = (f); dst[6] = (g); dst[7] = (h);	\
}

static void draw_texture(GLfloat *coor, GLfloat *vert)
{
	glTexCoordPointer(2, GL_FLOAT, 0, coor);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vert);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void draw_button(GLuint texid, GLfloat x, GLfloat y, GLfloat s, GLfloat *tex, GLfloat *ver)
{
	glBindTexture(GL_TEXTURE_2D, texid);
	// Texture から必要な部分を抜き出す(32x32を全部使う)
	SET_GLFLOATS(tex, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	// s倍にして貼り付ける
	SET_GLFLOATS(ver, x, y + 32.0f * s, x, y, x + 32.0f * s, y + 32.0f * s, x + 32.0f * s, y + 0.0f);
	draw_texture(tex, ver);
}

void draw_all_buttons(GLfloat *tex, GLfloat *ver, GLfloat scale, int32_t is_menu)
{
	int32_t i;
	VBTN_POINTS *p;

	p = Joystick_get_btn_points(scale);

	// 仮想キーはtexid: 1から6まで、キーボードonボタンが7、menuボタンが8
	for (i = 1; i < 9; i++) {
		if (need_Vpad() || i >= 7 || (Config.JoyOrMouse && i >= 5)) {
			draw_button(texid[i], p->x, p->y, scale, tex, ver);
		}
		p++;
	}
}
#endif // USE_OGLES11

/*==SDL2 Update Screen===*/
void
Update_Screen(uint32_t menu)
{
	SDL_Rect x68rect, CRTrect;

	/*surface(Main mem.) → texture(Frame buff.) */
	if(menu){
	  SDL_UpdateTexture(sdl_texture, NULL, menu_surface->pixels, 800*2);
	  x68rect.x = CRTrect.x = 0;
	  x68rect.y = CRTrect.y = 0;
	  x68rect.w = CRTrect.w = 800;
	  x68rect.h = CRTrect.h = 600;
	}
	else{
	  SDL_UpdateTexture(sdl_texture, NULL, ScrBuf, 800*2);
	  /*texture → renderer copy rect */
	  x68rect.x = 0;
	  x68rect.y = 0;
	  x68rect.w = TextDotX;
	  x68rect.h = TextDotY;
	  CRTrect.x = (800*drawW/surfaceW);
	  CRTrect.y = (600*drawH/surfaceH);
	  CRTrect.w = (800*TextDotX/surfaceW);
	  CRTrect.h = (600*TextDotY/surfaceH);
	}

	if(ScreenClearFlg != 0){ 			/*change screen(clear while 3 Frame)*/
	  SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 0);	/* select color (black) */
	  SDL_RenderClear(sdl_render);		/*renderer buffer clear*/
	  if(ScreenClearFlg == 2)	WinDraw_ChangeSizeGO();
	  if(ScreenClearFlg++ > 3) ScreenClearFlg=0;
	}

	SDL_RenderCopy(sdl_render, sdl_texture, &x68rect, &CRTrect);/*texture → renderer*/
	SDL_RenderPresent(sdl_render);								/* update screen */

 return;
}

/*描画バッファから表示バッファへの転送*/
/* X68K VRAM to Screen buffer   */
void FASTCALL
WinDraw_Draw(void)
{

	int32_t x,y;


#if defined(USE_OGLES11)
	GLfloat texture_coordinates[8];
	GLfloat vertices[8];
	GLfloat w;

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);

	// アルファブレンドしない(上のテクスチャが下のテクスチャを隠す)
	glBlendFunc(GL_ONE, GL_ZERO);

	glBindTexture(GL_TEXTURE_2D, texid[0]);
	//ScrBufから800x600の領域を切り出してテクスチャに転送
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	// magic numberがやたら多いが、テクスチャのサイズが1024x1024
	// OpenGLでの描画領域がglOrthof()で定義した800x600

	// X68K 画面描画

	// Texture から必要な部分を抜き出す
	// Texutre座標は0.0fから1.0fの間
	SET_GLFLOATS(texture_coordinates,
		     0.0f, (GLfloat)TextDotY/1024,
		     0.0f, 0.0f,
		     (GLfloat)TextDotX/1024, (GLfloat)TextDotY/1024,
		     (GLfloat)TextDotX/1024, 0.0f);

	// 実機の解像度(realdisp_w x realdisp_h)に関係なく、
	// 座標は800x600
	w = (realdisp_h * 1.33333) / realdisp_w * 800;
	SET_GLFLOATS(vertices,
		     (800.0f - w)/2, (GLfloat)600,
		     (800.0f - w)/2, 0.0f,
		     (800.0f - w)/2 + w, (GLfloat)600,
		     (800.0f - w)/2 + w, 0.0f);

	draw_texture(texture_coordinates, vertices);

	// ソフトウェアキーボード描画

	if (Keyboard_IsSwKeyboard()) {
		glBindTexture(GL_TEXTURE_2D, texid[10]);
		//kbd_bufferから800x600の領域を切り出してテクスチャに転送
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, kbd_buffer);

		// Texture から必要な部分を抜き出す
		// Texutre座標は0.0fから1.0fの間
		SET_GLFLOATS(texture_coordinates,
			     0.0f, (GLfloat)kbd_h/1024,
			     0.0f, 0.0f,
			     (GLfloat)kbd_w/1024, (GLfloat)kbd_h/1024,
			     (GLfloat)kbd_w/1024, 0.0f);

		// 実機の解像度(realdisp_w x realdisp_h)に関係なく、
		// 座標は800x600

		float kbd_scale = 0.8;
		SET_GLFLOATS(vertices,
			     (GLfloat)kbd_x, (GLfloat)(kbd_h * kbd_scale + kbd_y),
			     (GLfloat)kbd_x, (GLfloat)kbd_y,
			     (GLfloat)(kbd_w * kbd_scale + kbd_x), (GLfloat)(kbd_h * kbd_scale + kbd_y),
			     (GLfloat)(kbd_w * kbd_scale + kbd_x), (GLfloat)kbd_y);

		draw_texture(texture_coordinates, vertices);
	}

	// 仮想パッド/ボタン描画

	// アルファブレンドする(スケスケいやん)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	draw_all_buttons(texture_coordinates, vertices, (GLfloat)WinUI_get_vkscale(), 0);

	//	glDeleteTextures(1, &texid);

	SDL_GL_SwapWindow(sdl_window);

#else // end of OpenGLES

	Update_Screen(0); /* Draw Screen for X68000 (SDL2) */

#endif	// OpenGLES END

	FrameCount++;
	if (!Draw_DrawFlag/* && is_installed_idle_process()*/)
		return;
	Draw_DrawFlag = 0;

	if (SplashFlag)
		WinDraw_ShowSplash();
}


#define WD_MEMCPY(src) memcpy(&ScrBuf[adr], (src), TextDotX * 2)


#define WD_LOOP(startA, end, sub)			\
{ 							\
	for (i = (startA); i < (end); i++, adr++) {	\
		sub();					\
	}						\
}


#define WD_SUB(SUFFIX, src)			\
{						\
	w = (src);				\
	if (w != 0)				\
		ScrBuf##SUFFIX[adr] = w;	\
}


INLINE void WinDraw_DrawGrpLine(int32_t opaq)
{
#define _DGL_SUB(SUFFIX) WD_SUB(SUFFIX, Grp_LineBuf[i])

	int32_t adr;
	uint16_t w;
	uint32_t i;

	if(VLINE < 0) return;
	adr = VLINE*FULLSCREEN_WIDTH;

	if (opaq) {
		WD_MEMCPY(Grp_LineBuf);
	} else {
		WD_LOOP(0,  TextDotX, _DGL_SUB);
	}
}

INLINE void WinDraw_DrawGrpLineNonSP(int32_t opaq)
{
#define _DGL_NSP_SUB(SUFFIX) WD_SUB(SUFFIX, Grp_LineBufSP2[i])

	int32_t adr;
	uint16_t w;
	uint32_t i;

	if(VLINE < 0) return;
	adr = VLINE*FULLSCREEN_WIDTH;

	if (opaq) {
		WD_MEMCPY(Grp_LineBufSP2);
	} else {
		WD_LOOP(0,  TextDotX, _DGL_NSP_SUB);
	}
}

INLINE void WinDraw_DrawTextLine(int32_t opaq, int32_t td)
{
#define _DTL_SUB2(SUFFIX) WD_SUB(SUFFIX, BG_LineBuf[i])
#define _DTL_SUB(SUFFIX)		\
{					\
	if (Text_TrFlag[i] & 1) {	\
		_DTL_SUB2(SUFFIX);	\
	}				\
}	

	int32_t adr;
	uint16_t w;
	uint32_t i;

	if(VLINE < 0) return;
	adr = VLINE*FULLSCREEN_WIDTH;

	if (opaq) {
		WD_MEMCPY(&BG_LineBuf[16]);
	} else {
		if (td) {
			WD_LOOP(16, TextDotX + 16, _DTL_SUB);
		} else {
			WD_LOOP(16, TextDotX + 16, _DTL_SUB2);
		}
	}
}

INLINE void WinDraw_DrawTextLineTR(int32_t opaq)
{
#define _DTL_TR_SUB(SUFFIX)			   \
{						   \
	w = Grp_LineBufSP[i - 16];		   \
	if (w != 0) {				   \
		w &= Pal_HalfMask;		   \
		v = BG_LineBuf[i];		   \
		if (v & Ibit)			   \
			w += Pal_Ix2;		   \
		v &= Pal_HalfMask;		   \
		v += w;				   \
		v >>= 1;			   \
	} else {				   \
		if (Text_TrFlag[i] & 1)		   \
			v = BG_LineBuf[i];	   \
		else				   \
			v = 0;			   \
	}					   \
	ScrBuf##SUFFIX[adr] = (uint16_t)v;		   \
}

#define _DTL_TR_SUB2(SUFFIX)			   \
{						   \
	if (Text_TrFlag[i] & 1) {		   \
		w = Grp_LineBufSP[i - 16];	   \
		v = BG_LineBuf[i];		   \
						   \
		if (v != 0) {			   \
			if (w != 0) {			\
				w &= Pal_HalfMask;	\
				if (v & Ibit)		\
					w += Pal_Ix2;	\
				v &= Pal_HalfMask;	\
				v += w;			\
				v >>= 1;		\
			}				\
			ScrBuf##SUFFIX[adr] = (uint16_t)v;	\
		}					\
	}						\
}

	int32_t adr;
	int32_t v;
	uint16_t w;
	uint32_t i;

	if(VLINE < 0) return;
	adr = VLINE*FULLSCREEN_WIDTH;

	if (opaq) {
		WD_LOOP(16, TextDotX + 16, _DTL_TR_SUB);
	} else {
		WD_LOOP(16, TextDotX + 16, _DTL_TR_SUB2);
	}
}

INLINE void WinDraw_DrawBGLine(int32_t opaq, int32_t td)
{
#define _DBL_SUB2(SUFFIX) WD_SUB(SUFFIX, BG_LineBuf[i])
#define _DBL_SUB(SUFFIX)			 \
{						 \
	if (Text_TrFlag[i] & 2) {		 \
		_DBL_SUB2(SUFFIX); \
	} \
}

	int32_t adr;
	uint16_t w;
	uint32_t i;

	if(VLINE < 0) return;
	adr = VLINE*FULLSCREEN_WIDTH;

#if 0 // debug for segv
	static int32_t log_start = 0;

	if (TextDotX == 128 && TextDotY == 128) {
		log_start = 1;
	}
	if (log_start) {
		printf("opaq/td: %d/%d VLINE: %d, TextDotX: %d\n", opaq, td, VLINE, TextDotX);
	}
#endif

	if (opaq) {
		WD_MEMCPY(&BG_LineBuf[16]);
	} else {
		if (td) {
			WD_LOOP(16, TextDotX + 16, _DBL_SUB);
		} else {
			WD_LOOP(16, TextDotX + 16, _DBL_SUB2);
		}
	}
}

INLINE void WinDraw_DrawBGLineTR(int32_t opaq)
{

#define _DBL_TR_SUB3()			\
{					\
	if (w != 0) {			\
		w &= Pal_HalfMask;	\
		if (v & Ibit)		\
			w += Pal_Ix2;	\
		v &= Pal_HalfMask;	\
		v += w;			\
		v >>= 1;		\
	}				\
}

#define _DBL_TR_SUB(SUFFIX) \
{					\
	w = Grp_LineBufSP[i - 16];	\
	v = BG_LineBuf[i];		\
					\
	_DBL_TR_SUB3()			\
	ScrBuf##SUFFIX[adr] = (uint16_t)v;	\
}

#define _DBL_TR_SUB2(SUFFIX) \
{							\
	if (Text_TrFlag[i] & 2) {  			\
		w = Grp_LineBufSP[i - 16];		\
		v = BG_LineBuf[i];			\
							\
		if (v != 0) {				\
			_DBL_TR_SUB3()			\
			ScrBuf##SUFFIX[adr] = (uint16_t)v;	\
		}					\
	}						\
}

	int32_t adr;
	int32_t v;
	uint16_t w;
	uint32_t i;

	if(VLINE < 0) return;
	adr = VLINE*FULLSCREEN_WIDTH;

	if (opaq) {
		WD_LOOP(16, TextDotX + 16, _DBL_TR_SUB);
	} else {
		WD_LOOP(16, TextDotX + 16, _DBL_TR_SUB2);
	}

}

INLINE void WinDraw_DrawPriLine(void)
{
#define _DPL_SUB(SUFFIX) WD_SUB(SUFFIX, Grp_LineBufSP[i])

	int32_t adr;
	uint16_t w;
	uint32_t i;

	if(VLINE < 0) return;
	adr = VLINE*FULLSCREEN_WIDTH;

	WD_LOOP(0, TextDotX, _DPL_SUB);
}

void WinDraw_DrawLine(void)
{
	int32_t opaq, ton=0, gon=0, bgon=0, tron=0, pron=0, tdrawed=0;

	if((VLINE < 0) || (VLINE >= 1024)){ return; } /* Area check */
	if (!TextDirtyLine[VLINE]) return;
	TextDirtyLine[VLINE] = 0;
	Draw_DrawFlag = 1;


	if (Debug_Grp)
	{
	switch(VCReg0[1]&3)
	{
	case 0:					// 16 colors
		if (VCReg0[1]&4)		// 1024dot
		{
			if (VCReg2[1]&0x10)
			{
				if ( (VCReg2[0]&0x14)==0x14 )
				{
					Grp_DrawLine4hSP();
					pron = tron = 1;
				}
				else
				{
					Grp_DrawLine4h();
					gon=1;
				}
			}
		}
		else				// 512dot
		{
			if ( (VCReg2[0]&0x10)&&(VCReg2[1]&1) )
			{
				Grp_DrawLine4SP((VCReg1[1]   )&3/*, 1*/);			// 半透明の下準備
				pron = tron = 1;
			}
			opaq = 1;
			if (VCReg2[1]&8)
			{
				Grp_DrawLine4((VCReg1[1]>>6)&3, 1);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&4)
			{
				Grp_DrawLine4((VCReg1[1]>>4)&3, opaq);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&2)
			{
				if ( ((VCReg2[0]&0x1e)==0x1e)&&(tron) )
					Grp_DrawLine4TR((VCReg1[1]>>2)&3, opaq);
				else
					Grp_DrawLine4((VCReg1[1]>>2)&3, opaq);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&1)
			{
//				if ( (VCReg2[0]&0x1e)==0x1e )
//				{
//					Grp_DrawLine4SP((VCReg1[1]   )&3, opaq);
//					tron = pron = 1;
//				}
//				else
				if ( (VCReg2[0]&0x14)!=0x14 )
				{
					Grp_DrawLine4((VCReg1[1]   )&3, opaq);
					gon=1;
				}
			}
		}
		break;
	case 1:	
	case 2:	
		opaq = 1;		// 256 colors
		if ( (VCReg1[1]&3) <= ((VCReg1[1]>>4)&3) )	// 同じ値の時は、GRP0が優先（ドラスピ）
		{
			if ( (VCReg2[0]&0x10)&&(VCReg2[1]&1) )
			{
				Grp_DrawLine8SP(0);			// 半透明の下準備
				tron = pron = 1;
			}
			if (VCReg2[1]&4)
			{
				if ( ((VCReg2[0]&0x1e)==0x1e)&&(tron) )
					Grp_DrawLine8TR(1, 1);
				else if ( ((VCReg2[0]&0x1d)==0x1d)&&(tron) )
					Grp_DrawLine8TR_GT(1, 1);
				else
					Grp_DrawLine8(1, 1);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&1)
			{
				if ( (VCReg2[0]&0x14)!=0x14 )
				{
					Grp_DrawLine8(0, opaq);
					gon=1;
				}
			}
		}
		else
		{
			if ( (VCReg2[0]&0x10)&&(VCReg2[1]&1) )
			{
				Grp_DrawLine8SP(1);			// 半透明の下準備
				tron = pron = 1;
			}
			if (VCReg2[1]&4)
			{
				if ( ((VCReg2[0]&0x1e)==0x1e)&&(tron) )
					Grp_DrawLine8TR(0, 1);
				else if ( ((VCReg2[0]&0x1d)==0x1d)&&(tron) )
					Grp_DrawLine8TR_GT(0, 1);
				else
					Grp_DrawLine8(0, 1);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&1)
			{
				if ( (VCReg2[0]&0x14)!=0x14 )
				{
					Grp_DrawLine8(1, opaq);
					gon=1;
				}
			}
		}
		break;
	case 3:					// 65536 colors
		if (VCReg2[1]&15)
		{
			if ( (VCReg2[0]&0x14)==0x14 )
			{
				Grp_DrawLine16SP();
				tron = pron = 1;
			}
			else
			{
				Grp_DrawLine16();
				gon=1;
			}
		}
		break;
	}
	}


//	if ( ( ((VCReg1[0]&0x30)>>4) < (VCReg1[0]&0x03) ) && (gon) )
//		gdrawed = 1;				// GrpよりBGの方が上

	if ( ((VCReg1[0]&0x30)>>2) < (VCReg1[0]&0x0c) )
	{						// BGの方が上
		if ((VCReg2[1]&0x20)&&(Debug_Text))
		{
			Text_DrawLine(1);
			ton = 1;
		}
		else
			memset(Text_TrFlag, 0, TextDotX+16);

		if ((VCReg2[1]&0x40)&&(BG_Regs[8]&2)&&(!(BG_Regs[0x11]&2))&&(Debug_Sp))
		{
			int32_t s1, s2;
			s1 = (((BG_Regs[0x11]  &4)?2:1)-((BG_Regs[0x11]  &16)?1:0));
			s2 = (((CRTC_Regs[0x29]&4)?2:1)-((CRTC_Regs[0x29]&16)?1:0));
			VLINEBG = VLINE;
			VLINEBG <<= s1;
			VLINEBG >>= s2;
			if ( !(BG_Regs[0x11]&16) ) VLINEBG -= ((BG_Regs[0x0f]>>s1)-(CRTC_Regs[0x0d]>>s2));
			BG_DrawLine(!ton, 0);
			bgon = 1;
		}
	}
	else
	{						// Textの方が上
		if ((VCReg2[1]&0x40)&&(BG_Regs[8]&2)&&(!(BG_Regs[0x11]&2))&&(Debug_Sp))
		{
			int32_t s1, s2;
			s1 = (((BG_Regs[0x11]  &4)?2:1)-((BG_Regs[0x11]  &16)?1:0));
			s2 = (((CRTC_Regs[0x29]&4)?2:1)-((CRTC_Regs[0x29]&16)?1:0));
			VLINEBG = VLINE;
			VLINEBG <<= s1;
			VLINEBG >>= s2;
			if ( !(BG_Regs[0x11]&16) ) VLINEBG -= ((BG_Regs[0x0f]>>s1)-(CRTC_Regs[0x0d]>>s2));
			memset(Text_TrFlag, 0, TextDotX+16);
			BG_DrawLine(1, 1);
			bgon = 1;
		}
		else
		{
			if ((VCReg2[1]&0x20)&&(Debug_Text))
			{
				int_fast32_t i;
				for (i = 16; i < TextDotX + 16; ++i)
					BG_LineBuf[i] = TextPal[0];
			} else {		// 20010120 （琥珀色）
				memset(&BG_LineBuf[16], 0, TextDotX * 2);
			}
			memset(Text_TrFlag, 0, TextDotX+16);
			bgon = 1;
		}

		if ((VCReg2[1]&0x20)&&(Debug_Text))
		{
			Text_DrawLine(!bgon);
			ton = 1;
		}
	}


	opaq = 1;


#if 0
					// Pri = 3（違反）に設定されている画面を表示
		if ( ((VCReg1[0]&0x30)==0x30)&&(bgon) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x03)&&(tron) )
			{
				if ( (VCReg1[0]&3)<((VCReg1[0]>>2)&3) )
				{
					WinDraw_DrawBGLineTR(opaq);
					tdrawed = 1;
					opaq = 0;
				}
			}
			else
			{
				WinDraw_DrawBGLine(opaq, /*tdrawed*/0);
				tdrawed = 1;
				opaq = 0;
			}
		}
		if ( ((VCReg1[0]&0x0c)==0x0c)&&(ton) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x0c)&&(tron) )
				WinDraw_DrawTextLineTR(opaq);
			else
				WinDraw_DrawTextLine(opaq, /*tdrawed*/((VCReg1[0]&0x30)==0x30));
			opaq = 0;
			tdrawed = 1;
		}
#endif
					// Pri = 2 or 3（最下位）に設定されている画面を表示
					// プライオリティが同じ場合は、GRP<SP<TEXT？（ドラスピ、桃伝、YsIII等）

					// GrpよりTextが上にある場合にTextとの半透明を行うと、SPのプライオリティも
					// Textに引きずられる？（つまり、Grpより下にあってもSPが表示される？）

					// KnightArmsとかを見ると、半透明のベースプレーンは一番上になるみたい…。

		if ( (VCReg1[0]&0x02) )
		{
			if (gon)
			{
				WinDraw_DrawGrpLine(opaq);
				opaq = 0;
			}
			if (tron)
			{
				WinDraw_DrawGrpLineNonSP(opaq);
				opaq = 0;
			}
		}
		if ( (VCReg1[0]&0x20)&&(bgon) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x02)&&(tron) )
			{
				if ( (VCReg1[0]&3)<((VCReg1[0]>>2)&3) )
				{
					WinDraw_DrawBGLineTR(opaq);
					tdrawed = 1;
					opaq = 0;
				}
			}
			else
			{
				WinDraw_DrawBGLine(opaq, /*0*/tdrawed);
				tdrawed = 1;
				opaq = 0;
			}
		}
		if ( (VCReg1[0]&0x08)&&(ton) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x02)&&(tron) )
				WinDraw_DrawTextLineTR(opaq);
			else
				WinDraw_DrawTextLine(opaq, tdrawed/*((VCReg1[0]&0x30)>=0x20)*/);
			opaq = 0;
			tdrawed = 1;
		}

					// Pri = 1（2番目）に設定されている画面を表示
		if ( ((VCReg1[0]&0x03)==0x01)&&(gon) )
		{
			WinDraw_DrawGrpLine(opaq);
			opaq = 0;
		}
		if ( ((VCReg1[0]&0x30)==0x10)&&(bgon) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&(!(VCReg1[0]&0x03))&&(tron) )
			{
				if ( (VCReg1[0]&3)<((VCReg1[0]>>2)&3) )
				{
					WinDraw_DrawBGLineTR(opaq);
					tdrawed = 1;
					opaq = 0;
				}
			}
			else
			{
				WinDraw_DrawBGLine(opaq, ((VCReg1[0]&0xc)==0x8));
				tdrawed = 1;
				opaq = 0;
			}
		}
		if ( ((VCReg1[0]&0x0c)==0x04) && ((VCReg2[0]&0x5d)==0x1d) && (VCReg1[0]&0x03) && (((VCReg1[0]>>4)&3)>(VCReg1[0]&3)) && (bgon) && (tron) )
		{
			WinDraw_DrawBGLineTR(opaq);
			tdrawed = 1;
			opaq = 0;
			if (tron)
			{
				WinDraw_DrawGrpLineNonSP(opaq);
			}
		}
		else if ( ((VCReg1[0]&0x03)==0x01)&&(tron)&&(gon)&&(VCReg2[0]&0x10) )
		{
			WinDraw_DrawGrpLineNonSP(opaq);
			opaq = 0;
		}
		if ( ((VCReg1[0]&0x0c)==0x04)&&(ton) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&(!(VCReg1[0]&0x03))&&(tron) )
				WinDraw_DrawTextLineTR(opaq);
			else
				WinDraw_DrawTextLine(opaq, ((VCReg1[0]&0x30)>=0x10));
			opaq = 0;
			tdrawed = 1;
		}

					// Pri = 0（最優先）に設定されている画面を表示
		if ( (!(VCReg1[0]&0x03))&&(gon) )
		{
			WinDraw_DrawGrpLine(opaq);
			opaq = 0;
		}
		if ( (!(VCReg1[0]&0x30))&&(bgon) )
		{
			WinDraw_DrawBGLine(opaq, /*tdrawed*/((VCReg1[0]&0xc)>=0x4));
			tdrawed = 1;
			opaq = 0;
		}
		if ( (!(VCReg1[0]&0x0c)) && ((VCReg2[0]&0x5d)==0x1d) && (((VCReg1[0]>>4)&3)>(VCReg1[0]&3)) && (bgon) && (tron) )
		{
			WinDraw_DrawBGLineTR(opaq);
			tdrawed = 1;
			opaq = 0;
			if (tron)
			{
				WinDraw_DrawGrpLineNonSP(opaq);
			}
		}
		else if ( (!(VCReg1[0]&0x03))&&(tron)&&(VCReg2[0]&0x10) )
		{
			WinDraw_DrawGrpLineNonSP(opaq);
			opaq = 0;
		}
		if ( (!(VCReg1[0]&0x0c))&&(ton) )
		{
			WinDraw_DrawTextLine(opaq, 1);
			tdrawed = 1;
			opaq = 0;
		}

					// 特殊プライオリティ時のグラフィック
		if ( ((VCReg2[0]&0x5c)==0x14)&&(pron) )	// 特殊Pri時は、対象プレーンビットは意味が無いらしい（ついんびー）
		{
			WinDraw_DrawPriLine();
		}
		else if ( ((VCReg2[0]&0x5d)==0x1c)&&(tron) )	// 半透明時に全てが透明なドットをハーフカラーで埋める
		{						// （AQUALES）

#define _DL_SUB(SUFFIX) \
{								\
	w = Grp_LineBufSP[i];					\
	if (w != 0 && (ScrBuf##SUFFIX[adr] & 0xffff) == 0)	\
		ScrBuf##SUFFIX[adr] = (w & Pal_HalfMask) >> 1;	\
}

			int32_t adr;
			uint16_t w;
			uint32_t i;

			if(VLINE < 0) return;
			adr = VLINE*FULLSCREEN_WIDTH;

			WD_LOOP(0, TextDotX, _DL_SUB);
		}


	if (opaq)
	{
		int32_t adr;

		if(VLINE < 0) return;
		adr = VLINE*FULLSCREEN_WIDTH;

		memset(&ScrBuf[adr], 0, TextDotX * 2);

	}
}

/********** menu 関連ルーチン **********/

struct _px68k_menu {
	uint16_t *sbp;  // surface buffer ptr
	uint16_t *mlp; // menu locate ptr
	uint16_t mcolor; // color of chars to write
	uint16_t mbcolor; // back ground color of chars to write
	int32_t ml_x;
	int32_t ml_y;
	int32_t mfs; // menu font size;
} p6m;

// 画面タイプを変更する
enum ScrType {x68k, pc98};
int32_t scr_type = x68k;

/* sjis→jisコード変換 */
static uint16_t sjis2jis(uint16_t w)
{
	uint8_t wh, wl;

	wh = w / 256;
	wl = w % 256;

	wh <<= 1;
	if (wl < 0x9f) {
		wh += (wh < 0x3f)? 0x1f : -0x61;
		wl -= (wl > 0x7e)? 0x20 : 0x1f;
	} else {
		wh += (wh < 0x3f)? 0x20 : -0x60;
		wl -= 0x7e;
	}

	return (wh * 256 + wl);
}

/* JISコードから0 originのindexに変換する */
/* ただし0x2921-0x2f7eはX68KのROM上にないので飛ばす */
static uint16_t jis2idx(uint16_t jc)
{
	if (jc >= 0x3000) {
		jc -= 0x3021;
	} else {
		jc -= 0x2121;
	}
	jc = jc % 256 + (jc / 256) * 0x5e;

	return jc;
}

#define isHankaku(s) ((s) >= 0x20 && (s) <= 0x80 || (s) >= 0xa0 && (s) <= 0xdf)

#if defined(ANDROID)
// display width 800, buffer width 1024 だけれど 800 にしないとだめ
#define MENU_WIDTH 800
#else
#define MENU_WIDTH 800
#endif

// fs : font size : 8 or 16/24
// 半角文字の場合は16bitの上位8bitにデータを入れておくこと
// (半角or全角の判断ができるように)
static uint32_t get_font_addr(uint16_t sjis, int32_t fs)
{

	// 1Byte code
	uint8_t half_byte = (sjis >> 8);
	if (isHankaku(half_byte)) {
		switch (fs) {
		case 8:
			return (0x3a000 + half_byte * (1 * 8));
		case 16:
			return (0x3a800 + half_byte * (1 * 16));
		case 24:
			return (0x3d000 + half_byte * (2 * 24));
		default:
			return FALSE;
		}
	}

	// 2byte code(S-JIS)
	uint16_t jis = sjis2jis(sjis);
	uint16_t j_idx = (uint32_t)jis2idx(jis);
	uint8_t jhi = (uint8_t)(jis >> 8);
	int32_t fsb; // file size in bytes

	if (fs == 16) {
		fsb = 2 * 16;
	} else if (fs == 24) {
		fsb = 3 * 24;
	} else {
		return FALSE;
	}

#if 0
	printf("sjis code = 0x%x\n", sjis);
	printf("jis code = 0x%x\n", jis);
	printf("jhi 0x%x j_idx 0x%x\n", jhi, j_idx);
#endif

	if (jhi >= 0x21 && jhi <= 0x28) {
		// 非漢字
		return  ((fs == 16)? 0x0 : 0x40000) + j_idx * fsb;
	} else if (jhi >= 0x30 && jhi <= 0x74) {
		// 第一水準/第二水準
		return  ((fs == 16)? 0x5e00 : 0x4d380) + j_idx * fsb;
	}

	// ここにくることはないはず
	return FALSE;
}

// RGB565
static void set_mcolor(uint16_t c)
{
	p6m.mcolor = c;
}

// mbcolor = 0 なら透明色とする
static void set_mbcolor(uint16_t c)
{
	p6m.mbcolor = c;
}

// グラフィック座標
static void set_mlocate(int32_t x, int32_t y)
{
	p6m.ml_x = x, p6m.ml_y = y;
}

// キャラクタ文字の座標 (横軸は1座標が半角文字幅になる)
static void set_mlocateC(int32_t x, int32_t y)
{
	p6m.ml_x = x * p6m.mfs / 2, p6m.ml_y = y * p6m.mfs;
}

static void set_sbp(uint16_t *p)
{
	p6m.sbp = p;
}

// menu font size (16 or 24)
static void set_mfs(int32_t fs)
{
	p6m.mfs = fs;
}

static uint16_t *get_ml_ptr()
{
	p6m.mlp = p6m.sbp + MENU_WIDTH * p6m.ml_y + p6m.ml_x;
	return p6m.mlp;
}

// ・半角文字の場合は16bitの上位8bitにデータを入れておくこと
//   (半角or全角の判断ができるように)
// ・表示した分cursorは先に移動する
static void draw_char(uint16_t sjis)
{
	uint32_t f;
	uint16_t *p;
	int32_t i, j, k, wc, w;
	uint8_t c;
	uint16_t bc,ch;

	int32_t h = p6m.mfs;

	/*sjis範囲チェック*/
	ch=((sjis & 0xff00) >> 8);
	if((sjis & 0x00ff)==0x0000){
		if((ch<0x0020)||(ch>0x00df)){ return; }
	}
	else{
	  /*2byteチェック*/
	  if(((ch>=0x0081)&&(ch<=0x009f))||((ch>=0x00e0)&&(ch<=0x00ef))){
		bc=(sjis & 0x00ff);
		if((bc<0x0040)||(bc>0x00fc)){ return; }
	  }
	  else{
		return;
	  }
	}

	/*draw pointer*/
	p = get_ml_ptr();

	/*Font data address*/
	f = get_font_addr(sjis, h);
	if ((f == FALSE) || (f > 0x0c0000)){
		return;
	}

	// h=8は半角のみ
	w = (h == 8)? 8 : (isHankaku(sjis >> 8)? h / 2 : h);

	for (i = 0; i < h; i++) {
		wc = w;
		for (j = 0; j < ((w % 8 == 0)? w / 8 : w / 8 + 1); j++) {
			c = Memory_ReadB(f + 0xf00000);/*FONT-ROM*/
			f++;
			for (k = 0; k < 8 ; k++) {
				bc = p6m.mbcolor? p6m.mbcolor : *p;
				*p = (c & 0x80)? p6m.mcolor : bc;
				p++;
				c = c << 1;
				wc--;
				if (wc == 0)
					break;
			}
		}
		p = p + MENU_WIDTH - w;
	}

	p6m.ml_x += w;
}

/*--- 文字列を画面表示(sjis,utf8対応) ---*/
/*	flg = 0 sjis/utf8 (Auto)*/
/*	flg = 1 utf8 (Force)*/
static void draw_str(char *cp, uint32_t flg)
{
	uint32_t i, len, ret;
	uint8_t *s;
	uint16_t wc;
	char show_str[MAX_PATH];

	/*UTF8 or S-JIS*/
	ret = conv_utf8tosjis((char *)show_str, (char *)cp);
	if(flg==0){ /*auto select*/
		if(ret == 0) strcpy((char *)show_str, (char *)cp);
	}

	len = strlen((char *)show_str);
	s = (uint8_t *)show_str;

	for (i = 0; i < len; i++) {
		if (isHankaku(*s)) {
			// 最初の8bitで半全角を判断するので半角の場合は
			// あらかじめ8bit左シフトしておく
			draw_char((uint16_t)*s << 8);
			s++;
		} else {
			wc = (uint16_t)(*s << 8) + *(s + 1);
			draw_char(wc);
			s += 2;
			i++;
		}
		// 8x8描画(ソフトキーボードのFUNCキーは文字幅を縮める)
		if (p6m.mfs == 8) {
			p6m.ml_x -= 3;
		}
		/*Locate X dot check*/
		if((p6m.ml_x) > 740) break;
		/*Locate Y dot check*/
		if((p6m.ml_y) > 580) break;
	}

 return;
}

int32_t WinDraw_MenuInit(void)
{
#if defined(USE_OGLES11)
	//
	menu_buffer = malloc(1024 * 1024 * 2 + 2000);
	if (menu_buffer == NULL) {
		return FALSE;
	}
	set_sbp(menu_buffer);
	set_mfs(24);
#else

	/*SDL2 menu-surface */
	menu_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 16, WinDraw_Pal16R, WinDraw_Pal16G, WinDraw_Pal16B, 0);

	if (!menu_surface)
		return FALSE;
	set_sbp((uint16_t *)(menu_surface->pixels));
	set_mfs(24);
#endif

	set_mcolor(0xffff);
	set_mbcolor(0);

	return TRUE;
}

#include "menu_str_utf8.txt"

#ifdef USE_OGLES11
static void ogles11_draw_menu(void)
{
	GLfloat texture_coordinates[8];
	GLfloat vertices[8];

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, texid[9]);
	//ScrBufから800x600の領域を切り出してテクスチャに転送
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, menu_buffer);

	// Texture から必要な部分を抜き出す
	// Texutre座標は0.0fから1.0fの間
	SET_GLFLOATS(texture_coordinates,
		     0.0f, (GLfloat)600/1024,
		     0.0f, 0.0f,
		     (GLfloat)800/1024, (GLfloat)600/1024,
		     (GLfloat)800/1024, 0.0f);

	// 実機の解像度(realdisp_w x realdisp_h)に関係なく、
	// 座標は800x600
	SET_GLFLOATS(vertices,
		     40.0f, (GLfloat)600,
		     40.0f, 0.0f,
		     (GLfloat)800, (GLfloat)600,
		     (GLfloat)800, 0.0f);

	draw_texture(texture_coordinates, vertices);

	draw_all_buttons(texture_coordinates, vertices, (GLfloat)WinUI_get_vkscale(), 1);

	SDL_GL_SwapWindow(sdl_window);
}
#endif

/*==Menu描画==*/
void WinDraw_DrawMenu(int32_t menu_state, int32_t mkey_pos, int32_t mkey_y, int32_t *mval_y)
{
	int32_t i, drv;
	char tmp[256];

	SDL_RenderClear(sdl_render);/*screen buffer clear*/

// ソフトウェアキーボード描画時にset_sbp(kbd_buffer)されているので戻す
#if defined(USE_OGLES11)
	set_sbp(menu_buffer);
#endif
// ソフトウェアキーボード描画時にset_mfs(16)されているので戻す
#if defined(ANDROID) || TARGET_OS_IPHONE
	set_mfs(24);
#endif

	// タイトル
	if (scr_type == x68k) {
		set_mcolor(0x07ff); // cyan
		set_mlocateC(0, 0);
		draw_str(twaku_str,1);
		set_mlocateC(0, 1);
		draw_str(twaku2_str,1);
		set_mlocateC(0, 2);
		draw_str(twaku3_str,1);

		set_mcolor(0xffff);
		set_mlocateC(1, 1);
		sprintf(tmp, "%s%s", title_str, PX68KVERSTR);
		draw_str(tmp,1);
	} else {
		set_mcolor(0xffff);
		set_mlocateC(0, 0);
		sprintf(tmp, "%s%s", pc98_title1_str[Config.MenuLanguage], PX68KVERSTR);
		draw_str(tmp,1);
		set_mlocateC(0, 3);
		draw_str(pc98_title3_str[Config.MenuLanguage],1);
		set_mcolor(0x07ff);
		set_mlocateC(0, 1);
		draw_str(pc98_title2_str,1);
	}

	// 真ん中
	if (scr_type == x68k) {
		set_mcolor(0xffff);
		set_mlocate(3 * p6m.mfs / 2, 3.5 * p6m.mfs);
		draw_str(waku_val_str1[Config.MenuLanguage],1);
		set_mlocate(17 * p6m.mfs / 2, 3.5 * p6m.mfs);
		draw_str(waku_val_str2[Config.MenuLanguage],1);

		// 真ん中枠
		set_mcolor(0xffe0); // yellow
		set_mlocateC(1, 4);
		draw_str(waku_str,1);
		for (i = 5; i < 12; i++) {
			set_mlocateC(1, i);
			draw_str(waku2_str,1);
		}
		set_mlocateC(1, 12);
		draw_str(waku3_str,1);
	}

	// アイテム/キーワード
	set_mcolor(0xffff);
	for (i = 0; i < 7; i++) {
		set_mlocateC(3, 5 + i);
		if (menu_state == ms_key && i == (mkey_y - mkey_pos)) {
			set_mcolor(0x0);
			set_mbcolor(0xffe0); // yellow);
		} else {
			set_mcolor(0xffff);
			set_mbcolor(0x0);
		}
		draw_str(menu_item_key[i + mkey_pos],1);
	}

	// アイテム/現在値
	set_mcolor(0xffff);
	set_mbcolor(0x0);
	for (i = 0; i < 7; i++) {
		if ((menu_state == ms_value || menu_state == ms_hwjoy_set)
		    && i == (mkey_y - mkey_pos)) {
			set_mcolor(0x0);
			set_mbcolor(0xffe0); // yellow);
		} else {
			set_mcolor(0xffff);
			set_mbcolor(0x0);
		}
		if (scr_type == x68k) {
			set_mlocateC(17, 5 + i);
		} else {
			set_mlocateC(25, 5 + i);
		}

		drv = WinUI_get_drv_num(i + mkey_pos);
		if (drv >= 0  && mval_y[i + mkey_pos] == 0) {
			char *p;
			if (drv < 2) {
				p = (char *)Config.FDDImage[drv];
			} else {
				p = (char *)Config.HDImage[drv - 2];/*SASI*/
				if (p[0] == '\0') {
				  p = (char *)Config.SCSIEXHDImage[drv - 2];/*SCSI*/
				}
			}

			if (p[0] == '\0') {
				draw_str(" -- no disk --",1);
			} else {
			 // 先頭のカレントディレクトリ名を表示しない
			 if (!strncmp(cur_dir_str, p, cur_dir_slen)) {
				p += cur_dir_slen;
			 }
			 char str_sjis[MAX_PATH];
			 if(conv_utf8tosjis(str_sjis,p) == 0){strcpy(str_sjis,p);}/*sjisに変換*/
			 p=str_sjis;
			 if(strlen(p)>44){ /*24dotで44文字以上*/
			  set_mfs(16);
			  if(strlen(p)>66){/*16dotで66文字以上*/
			   p += (strlen(p)-66);
			  }
			 }
			 draw_str(p,0);
			 set_mfs(24);/*元に戻す*/
			}
		} else {
			draw_str(menu_items[i + mkey_pos][mval_y[i + mkey_pos]],1);
			/* Real Time Show change HwJoy Setting */
			if((i + mkey_pos)==11) 
			{
				switch(mval_y[i + mkey_pos])/*Axis0,1,Hat,Button0,1*/
				{
				case 0:/*Axis0 set*/
					if(JoyDirection == 0){
						sprintf(tmp," Right-Set:%d →",Config.HwJoyAxis[0]);
					}
					else{
						sprintf(tmp," Left -Set:%d ←",Config.HwJoyAxis[0]);
					}
					draw_str(tmp,1);
					break;
				case 1:/*Axis1 set*/
					if(JoyDirection == 0){
						sprintf(tmp," Down-Set:%d ↓",Config.HwJoyAxis[1]);
					}
					else{
						sprintf(tmp," Up  -Set:%d ↑",Config.HwJoyAxis[1]);
					}
					draw_str(tmp,1);
					break;
				case 2:/*hats set*/
					sprintf(tmp," Set:%d",Config.HwJoyHat);
					draw_str(tmp,1);
					break;
				case 3:/*Button0 set*/
					sprintf(tmp," Set ■:%d",Config.HwJoyBtn[0]);
					draw_str(tmp,1);
					break;
				case 4:/*Button1 set*/
					sprintf(tmp," Set ×:%d",Config.HwJoyBtn[1]);
					draw_str(tmp,1);
					break;
				default: break;
				}
			}
		}
	}

	if (scr_type == x68k) {
		// 下枠
		set_mcolor(0x07ff); // cyan
		set_mbcolor(0x0);
		set_mlocateC(0, 13);
		draw_str(swaku_str,1);
		set_mlocateC(0, 14);
		draw_str(swaku2_str,1);
		set_mlocateC(0, 15);
		draw_str(swaku2_str,1);
		set_mlocateC(0, 16);
		draw_str(swaku3_str,1);
	}

	// キャプション
	set_mcolor(0xffff);
	set_mbcolor(0x0);
	set_mlocateC(2, 14);
	if(Config.MenuLanguage == 0){draw_str(item_cap_JPN[mkey_y],1);}
	else						{draw_str(item_cap_US[mkey_y],1);}

	if (menu_state == ms_value) {
		set_mlocateC(2, 15);
	  if(Config.MenuLanguage == 0){draw_str(item_cap2_JPN[mkey_y],1);}
	  else						{draw_str(item_cap2_US[mkey_y],1);}
	}
#if defined(USE_OGLES11)
	ogles11_draw_menu();
#else

	Update_Screen(1); /* Draw Screen for Menu (SDL2) */

#endif
}

/*==File選択Menu描画==*/
void WinDraw_DrawMenufile(struct menu_flist *mfl)
{
	int32_t i;

	// 下枠
	//set_mcolor(0xf800); // red
	//set_mcolor(0xf81f); // magenta
	set_mcolor(0xffff);
	set_mbcolor(0x1); // 0x0だと透過モード
	set_mlocateC(1, 1);
	draw_str(swaku_str,1);
	for (i = 2; i < 16; i++) {
		set_mlocateC(1, i);
		draw_str(swaku2_str,1);
	}
	set_mlocateC(1, 16);
	draw_str(swaku3_str,1);

	for (i = 0; i < 14; i++) {
		if (i + 1 > mfl->num) {
			break;
		}
		if (i == mfl->y) {
			set_mcolor(0x0);
			set_mbcolor(0xffff);
		} else {
			set_mcolor(0xffff);
			set_mbcolor(0x1);
		}
		// ディレクトリだったらファイル名を[]で囲う
		set_mlocateC(3, i + 2);
		if (mfl->type[i + mfl->ptr]) draw_str("[",1);
		draw_str(mfl->name[i + mfl->ptr],0);
		if (mfl->type[i + mfl->ptr]) draw_str("]",1);
	}

	set_mbcolor(0x0); // 透過モードに戻しておく

#if defined(USE_OGLES11)
	ogles11_draw_menu();
#else

	Update_Screen(1); /* Draw Screen for Menu (SDL2) */

#endif
}

void WinDraw_ClearMenuBuffer(void)
{
#if defined(USE_OGLES11)
	memset(menu_buffer, 0, 800*600*2);
#else
	SDL_FillRect(menu_surface, NULL, 0);
#endif

}

/********** ソフトウェアキーボード描画 **********/

#if defined(USE_OGLES11)
// display width 800, buffer width 1024 だけれど 800 にしないとだめ
#define KBDBUF_WIDTH 800

#define KBD_FS 16 // keyboard font size : 16

// キーを反転する
void WinDraw_reverse_key(int32_t x, int32_t y)
{
	uint16_t *p;
	int32_t kp;
	int32_t i, j;
	
	kp = Keyboard_get_key_ptr(kbd_kx, kbd_ky);

	p = kbd_buffer + KBDBUF_WIDTH * kbd_key[kp].y + kbd_key[kp].x;

	for (i = 0; i < kbd_key[kp].h; i++) {
		for (j = 0; j < kbd_key[kp].w; j++) {
			*p = ~(*p);
			p++;
		}
		p = p + KBDBUF_WIDTH - kbd_key[kp].w;
	}
}

static void draw_kbd_to_tex()
{
	int32_t i, x, y;
	uint16_t *p;

	// SJIS 漢字コード
	char zen[] = {0x91, 0x53, 0x00};
	char larw[] = {0x81, 0xa9, 0x00};
	char rarw[] = {0x81, 0xa8, 0x00};
	char uarw[] = {0x81, 0xaa, 0x00};
	char darw[] = {0x81, 0xab, 0x00};
	char ka[] = {0x82, 0xa9, 0x00};
	char ro[] = {0x83, 0x8d, 0x00};
	char ko[] = {0x83, 0x52, 0x00};
	char ki[] = {0x8b, 0x4c, 0x00};
	char to[] = {0x93, 0x6f, 0x00};
	char hi[] = {0x82, 0xd0, 0x00};

	kbd_key[12].s = ka;
	kbd_key[13].s = ro;
	kbd_key[14].s = ko;
	kbd_key[16].s = ki;
	kbd_key[17].s = to;
	kbd_key[76].s = uarw;
	kbd_key[94].s = larw;
	kbd_key[95].s = darw;
	kbd_key[96].s = rarw;
	kbd_key[101].s = hi;
	kbd_key[108].s = zen;

	set_sbp(kbd_buffer);
	set_mfs(KBD_FS);
	set_mbcolor(0);
	set_mcolor(0);

	// キーボードの背景
	p = kbd_buffer;
	for (y = 0; y < kbd_h; y++) {
		for (x = 0; x < kbd_w; x++) {
			*p++ = (0x7800 | 0x03e0 | 0x000f);
		}
		p = p + KBDBUF_WIDTH - kbd_w;
	}

	// キーの描画
	for (i = 0; kbd_key[i].x != -1; i++) {
		p = kbd_buffer + kbd_key[i].y * KBDBUF_WIDTH + kbd_key[i].x;
		for (y = 0; y < kbd_key[i].h; y++) {
			for (x = 0; x < kbd_key[i].w; x++) {
				if (x == (kbd_key[i].w - 1)
				    || y == (kbd_key[i].h - 1)) {
					// キーに影をつけ立体的に見せる
					*p++ = 0x0000;
				} else {
					*p++ = 0xffff;
				}
			}
			p = p + KBDBUF_WIDTH - kbd_key[i].w;
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

	WinDraw_reverse_key(kbd_kx, kbd_ky);
}

#endif
