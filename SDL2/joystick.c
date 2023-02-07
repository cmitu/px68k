// JOYSTICK.C - joystick support for WinX68k

/*  refine for CyberStick Analog/Digital

CyberStick (Digital) PC4-L/H
D0:Stick UP     Throt UP
D1:     DOWN          DOWN
D2:     LEFT    Trig  C
D3:     RIGHT         D
D4:H
D5:Trig A(2)          E1
D6      B(1)          E2
D7:H

CyberStick(Analog)
 0: ACK L/H A   B   C   D
 1: 0   1   E1  E2  F   G
 2: 0   0   b7  b6  b5  b4 Axis Y
 3: 0   1   b7  b6  b5  b4 Axis X
 4: 0   0   b7  b6  b5  b4 Axis Z
 5: 0   1   b7  b6  b5  b4 (non)
 6: 0   0   b3  b2  b1  b0 Axis Y
 7: 0   1   b3  b2  b1  b0 Axis X
 8: 0   0   b3  b2  b1  b0 Axis Z
 9: 0   1   b3  b2  b1  b0 (non)
10: 0   0   A   B   A'  B'
11: 0   1   -   -   -   -
*/

#include "common.h"
#include "prop.h"
#include "joystick.h"
#include "winui.h"
#include "keyboard.h"

#if defined(ANDROID) || TARGET_OS_IPHONE
#include "mouse.h"
#endif

#ifndef MAX_BUTTON
#define MAX_BUTTON 32
#endif

char joyname[2][MAX_PATH];
char joybtnname[2][MAX_BUTTON][MAX_PATH];
uint8_t joybtnnum[2] = {0, 0};

uint8_t joy[2];
uint8_t JoyKeyState;
uint8_t JoyKeyState0;
uint8_t JoyKeyState1;
uint8_t JoyState0[2];
uint8_t JoyState1[2];

// for CyberStick
uint32_t CyberStick_mode;
uint8_t CyberST[12];
uint8_t CyberState;
uint32_t CyberTRX;
uint32_t CyberCount;
#define CyberACK 0x40;

#define SDL_AxisMIN (-32768)
#define SDL_AxisMAX (32767)

// This stores whether the buttons were down. This avoids key repeats.
uint8_t JoyDownState0;
uint8_t MouseDownState0;

// This stores whether the buttons were up. This avoids key repeats.
uint8_t JoyUpState0;
uint8_t MouseUpState0;

uint8_t JoyPortData[2];

#if defined(ANDROID) || TARGET_OS_IPHONE

#define VBTN_MAX 8
#define VBTN_NOUSE 0
#define FINGER_MAX 10

// The value is (0..1.0), which compares with SDL_FINGER.
typedef struct _vbtn_rect {
	float x;
	float y;
	float x2;
	float y2;
} VBTN_RECT;

VBTN_RECT vbtn_rect[VBTN_MAX];
uint8_t vbtn_state[VBTN_MAX];
SDL_TouchID touchId = -1;

#define SET_VBTN(id, bx, by, s)					\
{								\
	vbtn_state[id] = VBTN_OFF;				\
	vbtn_rect[id].x = (float)(bx) / 800.0;			\
	vbtn_rect[id].y = (float)(by) / 600.0;			\
	vbtn_rect[id].x2 = ((float)(bx) + 32.0*(s)) / 800.0;	\
	vbtn_rect[id].y2 = ((float)(by) + 32.0*(s)) / 600.0;	\
}

// base points of the virtual keys
VBTN_POINTS vbtn_points[] = {
	{20, 450}, {100, 450}, {60, 400}, {60, 500}, // d-pad
	{680, 450}, {750, 450}, // pad button
	{768, 0}, // software keyboard button
	{768, 52} // menu button
};

// fixed coordinates, which are fixed with any scalings.
#define VKEY_L_X 20 // x of the left d-pad key
#define VKEY_D_Y 532 // y of the base of the down d-pad key
#define VKEY_R_X 782 // x of the right side of the right pad button
#define VKEY_K_X 800 // x of the right side of keyboard button

// correction value for fixing keys
#define VKEY_DLX(scale) (VKEY_L_X * (scale) - VKEY_L_X)
#define VKEY_DDY(scale) (VKEY_D_Y * (scale) - VKEY_D_Y)
#define VKEY_DRX(scale) (VKEY_R_X * (scale) - VKEY_R_X)
#define VKEY_DKX(scale) (VKEY_K_X * (scale) - VKEY_K_X)

VBTN_POINTS scaled_vbtn_points[sizeof(vbtn_points)/sizeof(VBTN_POINTS)];

VBTN_POINTS *Joystick_get_btn_points(float scale)
{
	int_fast16_t i;

	for (i = 0; i < 4; i++) {
		scaled_vbtn_points[i].x
			= -VKEY_DLX(scale) + scale * vbtn_points[i].x;
		scaled_vbtn_points[i].y
			= -VKEY_DDY(scale) + scale * vbtn_points[i].y;
	}
	for (i = 4; i < 6; i++) {
		scaled_vbtn_points[i].x
			= -VKEY_DRX(scale) + scale * vbtn_points[i].x;
		scaled_vbtn_points[i].y
			= -VKEY_DDY(scale) + scale * vbtn_points[i].y;
	}

	// keyboard button
	scaled_vbtn_points[i].x
		= -VKEY_DKX(scale) + scale * vbtn_points[i].x;
	scaled_vbtn_points[i].y
		= 0;

	i++;

	// menu button
	scaled_vbtn_points[i].x
		= -VKEY_DKX(scale) + scale * vbtn_points[i].x;
	scaled_vbtn_points[i].y
		= 0 + scale * vbtn_points[i].y;

	return scaled_vbtn_points;
}

void Joystick_Vbtn_Update(float scale)
{
	int_fast16_t i;
	VBTN_POINTS *p;

	p = Joystick_get_btn_points(scale);

	for (i = 0; i < VBTN_MAX; i++) {
		vbtn_state[i] = VBTN_NOUSE;
		//p6logd("id: %d x: %f y: %f", i, p->x, p->y);
		SET_VBTN(i, p->x, p->y, scale);
		p++;
	}
}

uint8_t Joystick_get_vbtn_state(uint16_t n)
{
	return vbtn_state[n];
}

#endif

SDL_Joystick *sdl_joy;

void Joystick_Open(void)
{
	int32_t i, nr_joys, nr_axes, nr_btns, nr_hats;

	sdl_joy = 0;

	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	nr_joys = SDL_NumJoysticks();
	p6logd("joy num %d\n", nr_joys);

	if (nr_joys == 0){return;}

	for (i = 0; i < nr_joys; i++) {
		sdl_joy = SDL_JoystickOpen(i);
		if (sdl_joy) {
			nr_btns = SDL_JoystickNumButtons(sdl_joy);
			nr_axes = SDL_JoystickNumAxes(sdl_joy);
			nr_hats = SDL_JoystickNumHats(sdl_joy);

#if SDL_VERSION_ATLEAST(2, 0, 0)
			p6logd("Name: %s\n", SDL_JoystickNameForIndex(i));
#endif
			p6logd("# of Axes: %d\n", nr_axes);
			p6logd("# of Btns: %d\n", nr_btns);
			p6logd("# of Hats: %d\n", nr_hats);

			// skip accelerometer and keyboard
			if (nr_btns < 2 || (nr_axes < 2 && nr_hats == 0)) {
				Joystick_Cleanup();
				sdl_joy = 0;
			} else {
				break;
			}
		} else {
			p6logd("can't open joy %d\n", i);
		}
	}

 return;
}

void Joystick_Init(void)
{

	joy[0] = 1;  // active only one
	joy[1] = 0;
	JoyKeyState = 0;
	JoyKeyState0 = 0;
	JoyKeyState1 = 0;
	JoyState0[0] = 0xff;
	JoyState0[1] = 0xff;
	JoyState1[0] = 0xff;
	JoyState1[1] = 0xff;
	JoyPortData[0] = 0;
	JoyPortData[1] = 0;

	CyberStick_mode = 0;// CyberStickMode OFF

#if defined(ANDROID) || TARGET_OS_IPHONE
	Joystick_Vbtn_Update(WinUI_get_vkscale());
#endif

 return;
}


void Joystick_Cleanup(void)
{

#if SDL_VERSION_ATLEAST(2, 0, 0)
	if (SDL_JoystickGetAttached(sdl_joy)) {
		SDL_JoystickClose(sdl_joy);
	}
#else
	SDL_JoystickClose(sdl_joy);
#endif

}

uint8_t FASTCALL Joystick_Read(uint8_t num)
{
	uint8_t joynum = num;
	uint8_t ret0 = 0xff, ret1 = 0xff, ret;

	//+++ PC4が固定でReadが続く:デジタルPADと判定する +++
	if(num == 0){
	   if(CyberCount <  1000){ CyberCount++; }
	   if(CyberCount >= 1000){ CyberStick_mode = 0; }
	}

	//=== for CyberStick Mode (only PortA) ===
	if((CyberStick_mode == 1)&&(num == 0)){
		ret = CyberST[CyberState] | CyberACK;//set ACK=1
		switch(CyberTRX){
		 case 0:
		  CyberTRX++;// ACK=H
		  break;
		 case 1:
		  CyberTRX++;
		  ret &= ~CyberACK;// ACK=L
		  break;
		 case 2:
		  CyberTRX++;
		  ret &= ~CyberACK;// ACK=L(wait)
		  break;
		 case 3:
		  CyberTRX++;// ACK=H
		  break;
		 case 4:
		 default:
		  if(CyberState < 12){  CyberState++;  }
		  else{ ret = 0xff; }//ACH=H H/L=H data=all H
		  CyberTRX = 0;
		  break;
		}
	}
	else{//=== for ATARI Stick Mode ===
		if (Config.JoySwap) joynum ^= 1;
		if (joy[num]){
		  ret0 = JoyState0[num];
		  ret1 = JoyState1[num];
		}

		if (Config.JoyKey){
		  if ((Config.JoyKeyJoy2)&&(num==1))
		    ret0 ^= JoyKeyState;
		  else if ((!Config.JoyKeyJoy2)&&(num==0))
		    ret0 ^= JoyKeyState;
		}
	ret = ((~JoyPortData[num])&ret0)|(JoyPortData[num]&ret1);
	}
	return ret;
}

void FASTCALL Joystick_Write(uint8_t num, uint8_t data)
{
	if ( (num==0)||(num==1) ) JoyPortData[num] = data;

	if((data == 0x00)&&(num == 0)){// PortA PC4 H→L 
		if(CyberCount > 30) { CyberStick_mode = 1; }//CyberStick ON
		else { CyberStick_mode = 0; }				//CyberStick OFF
		CyberCount =0;
		CyberState = 0;
		CyberTRX = 0;
	}

}

void FASTCALL Joystick_Update(int32_t is_menu, SDL_Keycode key)
{
	uint8_t ret0 = 0xff, ret1 = 0xff;
	uint8_t mret0 = 0xff, mret1 = 0xff;
	int32_t num = 0; //xxx only joy1
	static uint8_t pre_ret0 = 0xff, pre_mret0 = 0xff;
	int32_t x, y, z;
	uint8_t x1, y1, z1, hat;

#if defined(ANDROID) || TARGET_OS_IPHONE
	SDL_Finger *finger;
	SDL_FingerID fid;
	float fx, fy;
	uint_fast16_t, j;
	float scale, asb_x, asb_y; // play x, play y of a button

	// all active buttons are set to off
	for (i = 0; i < VBTN_MAX; i++) {
		if (vbtn_state[i] != VBTN_NOUSE) {
			vbtn_state[i] = VBTN_OFF;
		}
	}

	if (touchId == -1)
		goto skip_vpad;

	// A play of the button changes size according to scale.
	scale = WinUI_get_vkscale();
	asb_x = (float)20 * scale / 800.0;
	asb_y = (float)20 * scale / 600.0;

	// set the button on, only which is pushed just now
	for (i = 0; i < FINGER_MAX; i++) {
		finger = SDL_GetTouchFinger(touchId, i);
		if (!finger)
			continue;

		fx = finger->x;
		fy = finger->y;

		//p6logd("id: %d x: %f y: %f", i, fx, fy);

		for (j = 0; j < VBTN_MAX; j++) {
			if (vbtn_state[j] == VBTN_NOUSE)
				continue;

			if (vbtn_rect[j].x - asb_x > fx)
				continue;
			if (vbtn_rect[j].x2 + asb_x < fx)
				continue;
			if (vbtn_rect[j].y - asb_y > fy)
				continue;
			if (vbtn_rect[j].y2 + asb_y < fy)
				continue;

			vbtn_state[j] = VBTN_ON;
			// The buttons don't overlap.
			break;
		}
	}

	if (need_Vpad()) {
		if (vbtn_state[0] == VBTN_ON) {
			ret0 ^= JOY_LEFT;
		}
		if (vbtn_state[1] == VBTN_ON) {
			ret0 ^= JOY_RIGHT;
		}
		if (vbtn_state[2] == VBTN_ON) {
			ret0 ^= JOY_UP;
		}
		if (vbtn_state[3] == VBTN_ON) {
			ret0 ^= JOY_DOWN;
		}
		if (vbtn_state[4] == VBTN_ON) {
			ret0 ^= (Config.VbtnSwap == 0)? JOY_TRG1 : JOY_TRG2;
		}
		if (vbtn_state[5] == VBTN_ON) {
			ret0 ^= (Config.VbtnSwap == 0)? JOY_TRG2 : JOY_TRG1;
		}
	} else if (Config.JoyOrMouse) {
		if (vbtn_state[4] == VBTN_ON) {
			mret0 ^= (Config.VbtnSwap == 0)? JOY_TRG1 : JOY_TRG2;
		}
		if (vbtn_state[5] == VBTN_ON) {
			mret0 ^= (Config.VbtnSwap == 0)? JOY_TRG2 : JOY_TRG1;
		}
	}

skip_vpad:

#endif // ANDROID || TARGET_OS_IPHONE

	// Hardware Joystick
	if (sdl_joy) {
		SDL_JoystickUpdate();
		x = SDL_JoystickGetAxis(sdl_joy, Config.HwJoyAxis[0]);
		y = SDL_JoystickGetAxis(sdl_joy, Config.HwJoyAxis[1]);
		z = SDL_JoystickGetAxis(sdl_joy, Config.HwJoyAxis[2]);

		/*0~255に正規化*/
		x1 = ((x-SDL_AxisMIN) * 255)/(SDL_AxisMAX-SDL_AxisMIN);
		y1 = ((y-SDL_AxisMIN) * 255)/(SDL_AxisMAX-SDL_AxisMIN);
		z1 = ((z-SDL_AxisMIN) * 255)/(SDL_AxisMAX-SDL_AxisMIN);

		CyberST[0] = 0x9f;
		CyberST[1] = 0xbf;
		CyberST[2] = 0x90 | (0x0f & (y1 >> 4));
		CyberST[3] = 0xb0 | (0x0f & (x1 >> 4));
		CyberST[4] = 0x90 | (0x0f & (z1 >> 4));
		CyberST[5] = 0xbf;
		CyberST[6] = 0x90 | (0x0f & y1 );
		CyberST[7] = 0xb0 | (0x0f & x1 );
		CyberST[8] = 0x90 | (0x0f & z1 );
		CyberST[9] = 0xbf;
		CyberST[10] = 0x9f;
		CyberST[11] = 0xbf;

		if (x < -JOYAXISPLAY) {
			ret0 ^= JOY_LEFT;
		}
		if (x > JOYAXISPLAY) {
			ret0 ^= JOY_RIGHT;
		}
		if (y < -JOYAXISPLAY) {
			ret0 ^= JOY_UP;
		}
		if (y > JOYAXISPLAY) {
			ret0 ^= JOY_DOWN;
		}

		hat = SDL_JoystickGetHat(sdl_joy, Config.HwJoyHat);

		if (hat) {
			switch (hat) {
			case SDL_HAT_RIGHTUP:
				ret0 ^= JOY_RIGHT;
			case SDL_HAT_UP:
				ret0 ^= JOY_UP;
				break;
			case SDL_HAT_RIGHTDOWN:
				ret0 ^= JOY_DOWN;
			case SDL_HAT_RIGHT:
				ret0 ^= JOY_RIGHT;
				break;
			case SDL_HAT_LEFTUP:
				ret0 ^= JOY_UP;
			case SDL_HAT_LEFT:
				ret0 ^= JOY_LEFT;
				break;
			case SDL_HAT_LEFTDOWN:
				ret0 ^= JOY_LEFT;
			case SDL_HAT_DOWN:
				ret0 ^= JOY_DOWN;
				break;
			}
		}

		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[0])) {
			ret0 ^= JOY_TRGA;
			CyberST[0] &= 0xf7;//A
			CyberST[10] &= 0xf7;
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[1])) {
			ret0 ^= JOY_TRGB;
			CyberST[0] &= 0xfb;//B
			CyberST[10] &= 0xfb;
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[2])) {
			ret1 ^= JOY_TRGC;
			CyberST[0] &= 0xfd;//C
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[3])) {
			ret1 ^= JOY_TRGD;
			CyberST[0] &= 0xfe;//D
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[4])) {
			ret1 ^= JOY_ThUP;//Th UP
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[5])) {
			ret1 ^= JOY_ThDN;//Th Down
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[6])) {
			ret1 ^= JOY_TRGE1;
			CyberST[1] &= 0xf7;//E1
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[7])) {
			ret1 ^= JOY_TRGE2;
			CyberST[1] &= 0xfb;//E2
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[8])) {
			CyberST[1] &= 0xfd;//F(Start)
		}
		if (SDL_JoystickGetButton(sdl_joy, Config.HwJoyBtn[9])) {
			CyberST[1] &= 0xfe;//G(Select)
		}
	}

	// scan keycode for menu UI
	if (key != SDLK_UNKNOWN) {
		switch (key) {
		case SDLK_UP :
			ret0 ^= JOY_UP;
			break;
		case SDLK_DOWN:
			ret0 ^= JOY_DOWN;
			break;
		case SDLK_LEFT:
			ret0 ^= JOY_LEFT;
			break;
		case SDLK_RIGHT:
			ret0 ^= JOY_RIGHT;
			break;
		case SDLK_RETURN:
			ret0 ^= JOY_TRGB;
			break;
		case SDLK_ESCAPE:
			ret0 ^= JOY_TRGA;
			break;
		}
	}

	JoyDownState0 = ~(ret0 ^ pre_ret0) | ret0;
	JoyUpState0 = (ret0 ^ pre_ret0) & ret0;
	pre_ret0 = ret0;

	MouseDownState0 = ~(mret0 ^ pre_mret0) | mret0;
	MouseUpState0 = (mret0 ^ pre_mret0) & mret0;
	pre_mret0 = mret0;

	// disable Joystick when software keyboard is active
	if (!is_menu && !Keyboard_IsSwKeyboard()) {
		JoyState0[num] = ret0;
		JoyState1[num] = ret1;
	}

#if defined(USE_OGLES11)
	// update the states of the mouse buttons
	if (!(MouseDownState0 & JOY_TRG1) | (MouseUpState0 & JOY_TRG1)) {
		printf("mouse btn1 event\n");
		Mouse_Event(1, (MouseUpState0 & JOY_TRG1)? 0 : 1.0, 0);
	}
	if (!(MouseDownState0 & JOY_TRG2) | (MouseUpState0 & JOY_TRG2)) {
		printf("mouse btn2 event\n");
		Mouse_Event(2, (MouseUpState0 & JOY_TRG2)? 0 : 1.0, 0);
	}
#endif
}

uint8_t get_joy_downstate(void)
{
	return JoyDownState0;
}
void reset_joy_downstate(void)
{
	JoyDownState0 = 0xff;
}
uint8_t get_joy_upstate(void)
{
	return JoyUpState0;
}
void reset_joy_upstate(void)
{
	JoyUpState0 = 0x00;
}

