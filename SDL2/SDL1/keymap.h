// 2022/6/4  SDL1専用テーブル

#define	NC	0
#define SCANTABLE_MAX 288

#define	SDL_SCANCODE_RIGHT	0x7c
#define	SDL_SCANCODE_LEFT	0x7b
#define	SDL_SCANCODE_UP		0x7e
#define	SDL_SCANCODE_DOWN	0x7d
#define	SDL_SCANCODE_Z		0x06
#define	SDL_SCANCODE_X		0x07

//SDL1 JIS-KeyBoardテーブル
uint8_t ScanTable[2][SCANTABLE_MAX] = {
{// Non shift-mode
	//	   a,   s,   d,   f,   h,   g,   z,   x		; 0x00
		0x1e,0x1f,0x20,0x21,0x23,0x22,0x2a,0x2b,
	//	   c,   v,   g,   b,   q,   w,   e,   r		; 0x08
		0x2c,0x2d,0x22,0x2e,0x11,0x12,0x13,0x14,
	//	   y,   t,   1,   2,   3,   4,   6,   5		; 0x10
		0x16,0x15,0x02,0x03,0x04,0x05,0x07,0x06,
	//	   ^,   9,   7,   -,  8,   0,   [,   o		; 0x18
		0x0d,0x0a,0x08,0x0c,0x09,0x0b,0x1c,0x19,
	//	   u,   @,   i,   p, ret,   l,   j,   :		; 0x20
		0x17,0x1b,0x18,0x1a,0x1d,0x26,0x24,0x28,
	//	   k,   ;,   ],   ,,   /,   n,   m,  .		; 0x28
		0x25,0x27,0x29,0x31,0x33,0x2f,0x30,0x32,
	//	 tab, spc,   `, del,  : , esc,  < ,  >		; 0x30
		0x10,0x35,0x0d,0x0f,0x28,0x01,0x31,0x32,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x38
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,   .,    ,   *,    ,   +,prsc,sclk		; 0x40
		  NC,0x51,  NC,0x41,  NC,0x46,  NC,  NC,
	//	    ,    ,    ,   /, ent,    ,   -,  		; 0x48
		0x61,0x5e,0x36,0x40,0x4e,0x3a,0x42,  NC,
	//	    ,   =,   0,   1,  2,  3 ,  4 ,  5		; 0x50(10key)
		  NC,0x4a,0x4f,0x4b,0x4c,0x4d,0x47,0x48,
	//	   6,   7,    ,   8,   9,   ¥,   ろ,  7		; 0x58
		0x49,0x43,  NC,0x44,0x45,0x0e,0x34,0x43,
	//	  F5,  F6,  F7,  F3,  F8,  F9,英数, F11		; 0x60
		0x67,0x68,0x69,0x65,0x6a,0x6b,  NC,0x5a,
	//	かな,    ,    ,    ,    , F10,    , F12		; 0x68
		  NC,  NC,  NC,  NC,  NC,0x6c,  NC,0x5b,
	//	    ,    ,    ,    ,    ,    ,  F4,  		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,0x66,  NC,
	//	  F2,    ,  F1,  ⬅︎,  ➡︎,  ⬇︎,  ⬆︎,    		; 0x78
		0x64,  NC,0x63,0x3b,0x3d,0x3e,0x3c,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x80(international)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,   ¥,    ,    ,    ,    ,    ,   		; 0x88(international)
		  NC,0x0e,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90(lang)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
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
	//	    ,    ,    ,    ,    ,CAPS,    , RFTL	; 0xe0
		  NC,  NC,  NC,  NC,  NC,0x5d,  NC, 0x70,
	//	SFTL,    ,CTRL,    , ALT,RCMD,LCMD,   		; 0xe8
		0x70,  NC,0x71,  NC,0x55,0x72,0x5f,  NC,
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
	//	   a,   s,   d,   f,   h,   g,   z,   x		; 0x00
		0x1e,0x1f,0x20,0x21,0x23,0x22,0x2a,0x2b,
	//	   c,   v,   g,   b,   q,   w,   e,   r		; 0x08
		0x2c,0x2d,0x22,0x2e,0x11,0x12,0x13,0x14,
	//	   y,   t,   1,   2,   3,   4,   6,   5		; 0x10
		0x16,0x15,0x02,0x03,0x04,0x05,0x07,0x06,
	//	   ^,   9,   7,   -,  8,   0,   [,   o		; 0x18
		0x0d,0x0a,0x08,0x0c,0x09,0x0b,0x1c,0x19,
	//	   u,   @,   i,   p, ret,   l,   j,   :		; 0x20
		0x17,0x1b,0x18,0x1a,0x1d,0x26,0x24,0x28,
	//	   k,   ;,   ],   ,,   /,   n,   m,  .		; 0x28
		0x25,0x27,0x29,0x31,0x33,0x2f,0x30,0x32,
	//	 tab, spc,   `, del,  : , esc,  < ,  >		; 0x30
		0x10,0x35,0x0d,0x0f,0x28,0x01,0x31,0x32,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x38
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,   .,    ,   *,    ,   +,prsc,sclk		; 0x40
		  NC,0x51,  NC,0x41,  NC,0x46,  NC,  NC,
	//	    ,    ,    ,   /, ent,    ,   -,  		; 0x48
		0x61,0x5e,0x36,0x40,0x4e,0x3a,0x42,  NC,
	//	    ,   =,   0,   1,  2,  3 ,  4 ,  5		; 0x50(10key)
		  NC,0x4a,0x4f,0x4b,0x4c,0x4d,0x47,0x48,
	//	   6,   7,    ,   8,   9,   ¥,   ろ,  7		; 0x58
		0x49,0x43,  NC,0x44,0x45,0x0e,0x34,0x43,
	//	  F5,  F6,  F7,  F3,  F8,  F9,英数, F11		; 0x60
		0x67,0x68,0x69,0x65,0x6a,0x6b,  NC,0x5a,
	//	かな,    ,    ,    ,    , F10,    , F12		; 0x68
		  NC,  NC,  NC,  NC,  NC,0x6c,  NC,0x5b,
	//	    ,    ,    ,    ,    ,    ,  F4,  		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,0x66,  NC,
	//	  F2,    ,  F1,  ⬅︎,  ➡︎,  ⬇︎,  ⬆︎,    		; 0x78
		0x64,  NC,0x63,0x3b,0x3d,0x3e,0x3c,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x80(international)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,   ¥,    ,    ,    ,    ,    ,   		; 0x88(international)
		  NC,0x0e,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90(lang)
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
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
	//	    ,    ,    ,    ,    ,CAPS,    , RFTL	; 0xe0
		  NC,  NC,  NC,  NC,  NC,0x5d,  NC, 0x70,
	//	SFTL,    ,CTRL,    , ALT,RCMD,LCMD,   		; 0xe8
		0x70,  NC,0x71,  NC,0x55,0x72,0x5f,  NC,
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

