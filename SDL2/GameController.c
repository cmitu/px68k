/* GameController.c - XBOX like GamePad Support.
   as CyberStick Analog/Digital mode

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
#include "GameController.h"
#include "winui.h"
#include "keyboard.h"
#include "dosio.h"

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
uint8_t CyberST[14];
uint8_t CyberState;
uint32_t CyberTRX;
uint32_t CyberCount;
#define CyberACK 0x40;

// for Axis MAX,MIN
#define SDL_AxisMIN -32768
#define SDL_AxisMAX  32767

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

SDL_GameController *sdl_gamepad;
SDL_Joystick *sdl_joy; //DUMMY
static uint32_t	cyber_tick = 0;//時間計測用

void GameController_Open(void)
{
	static const char gamepaddb_filename[] = "gamecontrollerdb.txt";
	const char *gamepad_db;

	int32_t  nr_prod, nr_vers, nr_btns, nr_joys;
	const char *name;
	int i,num;

	SDL_Init(SDL_INIT_GAMECONTROLLER);

	// GamePad をイベントで駆動する
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_GameControllerEventState(SDL_ENABLE);

	gamepad_db = File_Getcd((char *)gamepaddb_filename);
	num = SDL_GameControllerAddMappingsFromFile((char *)gamepad_db);
	if(num > 0) p6logd("%d Game Controller mapped.\n",num);

	sdl_gamepad = NULL;

	nr_joys = SDL_NumJoysticks();
	if (nr_joys == 0){ return; }

    for (i = 0; i < 1; ++i) {// 1個だけサポート
		if ( SDL_IsGameController(i) )
		{
			name = SDL_GameControllerNameForIndex(i);
			//p6logd("Game Controller %d: %s\n", i, name ? name : "Unknown Controller");
			sdl_gamepad = SDL_GameControllerOpen(i);
			p6logd("GameController No.%d %s connected.\n", i,name);
		}
		else{
		  p6logd("No.%d Not compatible GameController interface.\n", i);
		}
    }

 return;
}


// 変数初期化
void GameController_Init(void)
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

	CyberST[0] = 0x9f;
	CyberST[1] = 0xbf;
	CyberST[2] = 0x90 | 0x07;
	CyberST[3] = 0xb0 | 0x07;
	CyberST[4] = 0x90 | 0x07;
	CyberST[5] = 0xb7;
	CyberST[6] = 0x90 | 0x0f;
	CyberST[7] = 0xb0 | 0x0f;
	CyberST[8] = 0x90 | 0x0f;
	CyberST[9] = 0xbf;
	CyberST[10] = 0x9f;
	CyberST[11] = 0xbf;
	CyberST[13] = 0x9f | CyberACK;// dummy

	CyberStick_mode = 0;// CyberStickMode OFF

  return;
}

// GameController close
void GameController_Cleanup(void)
{
	if(sdl_gamepad){
		SDL_GameControllerClose(sdl_gamepad);
	}
}

// Read from X68000
uint8_t FASTCALL Joystick_Read(uint8_t num)
{
	uint8_t joynum = num;
	uint8_t ret0 = 0xff, ret1 = 0xff, ret;

	//+++ 自動判定:PC4が固定でReadが続く:デジタルPADと判定する +++
	if(num == 0){
	 if(Config.Joymode == 0){ CyberStick_mode = 0; }// 強制DIGITAL
	 else if(Config.Joymode == 1){ CyberStick_mode = 1;}// 強制ANALOG
	 else if(Config.Joymode == 2){//自動判定
	   if(CyberCount <  1000){ CyberCount++; }
	   if(CyberCount >= 1000){ CyberStick_mode = 0; }
	 }
	}

	//=== for CyberStick Mode ===
	if((CyberStick_mode == 1)&&(num == 0)){

	//printf("Cyber %d  %d %d,%x %x\n",CyberStick_mode,CyberCount,CyberState,CyberST[CyberState],JoyPortData[0]);

		ret = 0x9f | CyberACK;//set ACK=H
		switch(CyberTRX){
		 case 0:
		  ret = 0x9f | CyberACK;//set ACK=H
		  uint32_t ticknow = Get_usecCount();//1μs単位
		  uint32_t dif;
		  if(ticknow>=cyber_tick){dif = ticknow-cyber_tick; }
		  else{dif = 0xffffffff-cyber_tick+ticknow;}//補正
		  if(dif>40){
		    CyberTRX++;
		  }
		  break;
		 case 1:
		  ret = CyberST[CyberState] | CyberACK;//set ACK=H
		  CyberTRX++;
		  break;
		 case 2:
		  ret = CyberST[CyberState];// ACK=L
		  CyberTRX++;
		  break;
		 case 3:
		  ret = CyberST[CyberState] | CyberACK;//set ACK=H
		  CyberTRX++;
		  CyberState++;
		  break;
		 case 4:
		  ret = CyberST[CyberState] | CyberACK;//set ACK=H
		  CyberTRX++;
		  break;
		 case 5:
		  ret = CyberST[CyberState];//set ACK=L
		  CyberTRX++;
		  break;
		 case 6:
		  ret = CyberST[CyberState] | CyberACK;//set ACK=H
		  CyberTRX++;
		  CyberState++;
		  break;
		 case 7:
		  ret = CyberST[CyberState];//set ACK=L
		  CyberTRX++;
		  break;
		 default:
		  ret = CyberST[CyberState];//set ACK=L
		  if(CyberState < 12){  CyberTRX = 1;  }
		  else{ ret = 0x9f | CyberACK; }//ACH=H H/L=H data=all H
		  break;
		}
   }
   else{//=== for ATARI Stick Mode ===
		if (Config.JoySwap) joynum ^= 1;
		if (joy[num]) {
		  ret0 = JoyState0[num];
		  ret1 = JoyState1[num];
		}

		if (Config.JoyKey)
		{
		  if ((Config.JoyKeyJoy2)&&(num==1))
		    ret0 ^= JoyKeyState;
		  else if ((!Config.JoyKeyJoy2)&&(num==0))
		    ret0 ^= JoyKeyState;
		}

		ret = ((~JoyPortData[num])&ret0)|(JoyPortData[num]&ret1);
   }

	return ret;

}

// Write from X68000
void FASTCALL Joystick_Write(uint8_t num, uint8_t data)
{
	if(num == 0){// PortA
		if((JoyPortData[num]==0xff)&&(data == 0x00)){// PortA PC4 H→L
		  if(CyberCount > 300) { CyberStick_mode = 1; }//CyberStick ON
		  else { CyberStick_mode = 0; }				//CyberStick OFF
		  CyberCount =0;
		  CyberState = 0;
		  CyberTRX = 0;
		  cyber_tick = Get_usecCount();//1μs単位
		}
	}

	if ( (num==0)||(num==1) ) JoyPortData[num] = data;
}

// GamePad Analog input for X68000 (like CyberKtick)
void FASTCALL GameControllerAxis_Update(void)
{
	int32_t num = 0; //xxx only joy1
	int16_t x, y, z;
	uint8_t x1, y1, z1;

	// XBOX like GamePad
	if (sdl_gamepad) {
		//SDL_GameControllerUpdate();
		x = SDL_GameControllerGetAxis(sdl_gamepad, SDL_CONTROLLER_AXIS_LEFTX);
		y = SDL_GameControllerGetAxis(sdl_gamepad, SDL_CONTROLLER_AXIS_LEFTY);
		z = SDL_GameControllerGetAxis(sdl_gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT);

		/*0~255に正規化*/
		x1 = ((x-SDL_AxisMIN) * 255)/(SDL_AxisMAX-SDL_AxisMIN);
		y1 = ((y-SDL_AxisMIN) * 255)/(SDL_AxisMAX-SDL_AxisMIN);
		z1 = ((z-SDL_AxisMIN) * 255)/(SDL_AxisMAX-SDL_AxisMIN);

		//CyberST[0] = 0x9f;
		//CyberST[1] = 0xbf;
		CyberST[2] = 0x90 | (0x0f & (y1 >> 4));
		CyberST[3] = 0xb0 | (0x0f & (x1 >> 4));
		CyberST[4] = 0x90 | (0x0f & (z1 >> 4));
		CyberST[5] = 0xbf;
		CyberST[6] = 0x90 | (0x0f & y1 );
		CyberST[7] = 0xb0 | (0x0f & x1 );
		CyberST[8] = 0x90 | (0x0f & z1 );
		CyberST[9] = 0xbf;
		//CyberST[10] = 0x9f;
		CyberST[11] = 0xbf;
		CyberST[13] = 0x9f | CyberACK;// dummy
	}

 return;
}

// GamePad Button for X68000 
void FASTCALL GameControllerButton_Update(int32_t is_menu)
{
	uint8_t ret0 = 0xff, ret1 = 0xff;
	int32_t num = 0; //xxx only joy1

	int16_t x, y, z;
	uint8_t x1, y1, z1;

	// XBOX like GamePad
	if (sdl_gamepad) {

		// Atari Digital +
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1){
		  ret0 ^= JOY_UP;
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1){
		  ret0 ^= JOY_DOWN;
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1){
		  ret0 ^= JOY_LEFT;
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1){
		  ret0 ^= JOY_RIGHT;
		}

		// Atari A B C D E1 E2  Start Select
		CyberST[0]  = 0x9f; // init A B C D
		CyberST[1]  = 0xbf; // init E1 E2 F G
		CyberST[10] = 0x9f; // init A B A'B'
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_A) == 1){//A
		  ret0 ^= JOY_TRGA;
		  CyberST[0] &= 0xf7;//A
		  CyberST[10] &= 0xf7;
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_B) == 1){//B
		  ret0 ^= JOY_TRGB;
		  CyberST[0] &= 0xfb;//B
		  CyberST[10] &= 0xfb;
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_X) == 1){//X
		  ret1 ^= JOY_TRGC;
		  CyberST[0] &= 0xfd;//C
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_Y) == 1){//Y
		  ret1 ^= JOY_TRGC;
		  CyberST[0] &= 0xfe;//D
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1){//Th up
		  ret1 ^= JOY_ThUP;//Th UP
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1){//Th down
		  ret1 ^= JOY_ThDN;//Th Down
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1){//E1
		  ret1 ^= JOY_TRGE1;
		  CyberST[1] &= 0xf7;//E1
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1){//E2
		  ret1 ^= JOY_TRGE2;
		  CyberST[1] &= 0xfb;//E2
		}

		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_START) == 1){//F(Start)
		  CyberST[1] &= 0xfd;//F(Start)
		}
		if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_BACK) == 1){//G(Select)
		  CyberST[1] &= 0xfe;//G(select)
		}

	}

	/*JoyDownState0 = ~(ret0 ^ pre_ret0) | ret0;
	JoyUpState0 = (ret0 ^ pre_ret0) & ret0;
	pre_ret0 = ret0;*/

	// disable Joystick when software keyboard is active
	if ( !is_menu && !Keyboard_IsSwKeyboard()) {
		JoyState0[num] = ret0;
		JoyState1[num] = ret1;
	}

 return;
}

// Keyinput and GamePad for Menu Mode 
void FASTCALL Menu_GameController_Update(SDL_Keycode key)
{
	uint8_t ret0 = 0xff;
	uint8_t ret1 = 0xff;
	static uint8_t pre_ret0 = 0xff, pre_mret0 = 0xff;
	int32_t num = 0; //xxx only joy1

	// Atari Digital +
	if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1){//+UP
	  ret0 ^= JOY_UP;
	}
	if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1){//+DOWN
	  ret0 ^= JOY_DOWN;
	}
	if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1){//+LEFT
	  ret0 ^= JOY_LEFT;
	}
	if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1){//+RIGHT
	  ret0 ^= JOY_RIGHT;
	}

	if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_A) == 1){//A
	  ret0 ^= JOY_TRGA;
	}
	if(SDL_GameControllerGetButton(sdl_gamepad, SDL_CONTROLLER_BUTTON_B) == 1){//B
	  ret0 ^= JOY_TRGB;
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
			ret0 ^= JOY_TRGA;
			break;
		case SDLK_ESCAPE:
			ret0 ^= JOY_TRGB;
			break;
		}
	}

	JoyDownState0 = ~(ret0 ^ pre_ret0) | ret0;
	JoyUpState0 = (ret0 ^ pre_ret0) & ret0;
	pre_ret0 = ret0;

 return;
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

