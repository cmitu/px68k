// 2022/5/8  XKBは削除 SDL2専用テーブルに作り換え
// SDL2共通scancodeキーコード 0~284(0x11c)変換テーブル

#define	NC	0
#define SCANTABLE_MAX 288

//SDL2 JIS-Keyboard 変換テーブル
uint8_t ScanTable[2][SCANTABLE_MAX] = {
{// Non shift-mode
	//	ukwn,    ,    ,    ,   a,   b,   c,   d		; 0x00
		  NC,  NC,  NC,  NC,0x1e,0x2e,0x2c,0x20,
	//	   e,   f,   g,   h,   i,   j,   k,   l		; 0x08
		0x13,0x21,0x22,0x23,0x18,0x24,0x25,0x26,
	//	   m,   n,   o,   p,   q,   r,   s,   t		; 0x10
		0x30,0x2f,0x19,0x1a,0x11,0x14,0x1f,0x15,
	//	   u,   v,   w,   x,   y,   z,   1,   2		; 0x18
		0x17,0x2d,0x12,0x2b,0x16,0x2a,0x02,0x03,
	//	   3,   4,   5,   6,   7,   8,   9,   0		; 0x20
		0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,
	//	 ret, esc, del, tab, spc,  - ,  ^ ,  @		; 0x28
		0x1d,0x01,0x0f,0x10,0x35,0x0c,0x0d,0x1b,
	//	  [ ,  ] ,    ,  ; ,  : ,    ,  , ,  .		; 0x30
		0x1c,0x29,  NC,0x27,0x28,  NC,0x31,0x32,
	//	  / , cap,  F1,  F2,  F3,  F4,  F5,  F6		; 0x38
		0x33,0x5d,0x63,0x64,0x65,0x66,0x67,0x68,
	//	  F7,  F8,  F9, F10, F11, F12,prsc,sclk		; 0x40
		0x69,0x6a,0x6b,0x6c,  NC,  NC,  NC,  NC,
	//	paus, ins,home,pgup, del, end,pgdw, →		; 0x48
		0x61,0x5e,0x36,0x38,0x37,0x3a,0x39,0x3d,
	//	  ←,   ↓,   ↑, numl,  /,  * ,  - ,  +		; 0x50(10key)
		0x3b,0x3e,0x3c,0x3f,0x40,0x41,0x42,0x46,
	//	 ent,   1,   2,   3,   4,   5,   6,  7		; 0x58(10key)
		0x4e,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x43,
	//	   8,   9,   0,   .,  ろ,    ,    , =		; 0x60(JISwinろ)
		0x44,0x45,0x4f,0x51,0x34,  NC,  NC,0x4a,
	//	 F13, F14, F15,    ,    ,    ,    ,  		; 0x68
		0x5c,0x52,0x54,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    , stop	; 0x78
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,0x61,
	//	    ,    ,    ,    ,    ,    ,    ,  ろ		; 0x80(international)(JISmacろ)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,0x34,
	//	    ,   ¥,    ,    ,    ,    ,    ,   		; 0x88(international)
		  NC,0x0e,  NC,  NC,  NC,  NC,  NC,  NC,
	//	 かな,英数,    ,    ,    ,    ,    ,  		; 0x90(lang)
		0x5a,0x5b,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
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
	//	ctrl,sftL,altL,cmdL,ctrl,sftR,altR,cmdR		; 0xe0
		0x71,0x70,0x55,0x5f,0x71,0x70,0x59,0x72,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x100
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x108
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x110
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x118(0x11c)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
},
{// shift-mode
	//	ukwn,    ,    ,    ,   A,   B,   C,   D		; 0x00
		  NC,  NC,  NC,  NC,0x1e,0x2e,0x2c,0x20,
	//	   E,   F,   G,   H,   I,   J,   K,   L		; 0x08
		0x13,0x21,0x22,0x23,0x18,0x24,0x25,0x26,
	//	   M,   N,   O,   P,   Q,   R,   S,   T		; 0x10
		0x30,0x2f,0x19,0x1a,0x11,0x14,0x1f,0x15,
	//	   U,   V,   W,   X,   Y,   Z,   1,   2		; 0x18
		0x17,0x2d,0x12,0x2b,0x16,0x2a,0x02,0x03,
	//	   3,   4,   5,   6,   7,   8,   9,   0		; 0x20
		0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,
	//	 ret, esc, del, tab, spc,  = ,  ~ ,  `		; 0x28
		0x1d,0x01,0x0f,0x10,0x35,0x0c,0x0d,0x1b,
	//	  { ,  } ,    ,  + ,  * ,    ,  < ,  >		; 0x30
		0x1c,0x29,  NC,0x27,0x28,  NC,0x31,0x32,
	//	  ? , cap,  F1,  F2,  F3,  F4,  F5,  F6		; 0x38
		0x33,0x5d,0x63,0x64,0x65,0x66,0x67,0x68,
	//	  F7,  F8,  F9, F10,  NC,  NC,prsc,sclk		; 0x40
		0x69,0x6a,0x6b,0x6c,0x5a,0x5b,  NC,  NC,
	//	paus, ins,home,pgup, del, end,pgdw, →		; 0x48
		0x61,0x5e,0x36,0x38,0x37,0x3a,0x39,0x3d,
	//	  ←,   ↓,   ↑, numl,  /,  * ,  - ,  +		; 0x50(10key)
		0x3b,0x3e,0x3c,0x3f,0x40,0x41,0x42,0x46,
	//	 ent,   1,   2,   3,   4,   5,   6,  7		; 0x58(10key)
		0x4e,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x43,
	//	   8,   9,   0,   .,  ろ,    ,    , =		; 0x60(JISwinろ)
		0x44,0x45,0x4f,0x51,0x34,  NC,  NC,0x4a,
	//	 F13, F14, F15,    ,    ,    ,    ,  		; 0x68
		0x5c,0x52,0x54,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    , stop	; 0x78
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,0x61,
	//	    ,    ,    ,    ,    ,    ,    ,  ろ		; 0x80(international)(JISmacろ)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,0x34,
	//	    ,   |,    ,    ,    ,    ,    ,   		; 0x88(international)
		  NC,0x0e,  NC,  NC,  NC,  NC,  NC,  NC,
	//	 かな,英数,    ,    ,    ,    ,    ,   		; 0x90(lang)
		0x5a,0x5b,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
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
	//	ctrl,sftL,altL,cmdL,ctrl,sftR,altR,cmdR		; 0xe0
		0x71,0x70,0x55,0x5f,0x71,0x70,0x59,0x73,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x100
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x108
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x110
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x118(0x11c)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
}
};

