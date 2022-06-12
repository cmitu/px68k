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
	//	    ,   ¥,    ,    ,    ,    ,    ,    		; 0xa0(JIS Keyboard)
		  NC,0x0e,  NC,  NC,  NC,  NC,  NC,  NC,
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
	//	  F7,  F8,  F9, F10,    ,    ,    ,    		; 0xe0
		0x69,0x6a,0x6b,0x6c,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    , cap,    ,sftR		; 0xe8
		  NC,  NC,  NC,  NC,  NC,0x5d,  NC,0x70,
	//	sftL,    , ctr,    , opt,cmdR,cmdL,    		; 0xf0
		0x70,  NC,0x71,  NC,0x5b,0x5b,0x5a,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x100
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x108
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,   ↑,   ↓,   →,  ←,    ,    ,    		; 0x110
		  NC,0x3c,0x3e,0x3d,0x3b,  NC,  NC,  NC,
	//	    ,    ,  F1,  F2,  F3,  F4,  F5,  F6		; 0x118(0x11c)
		  NC,  NC,0x63,0x64,0x65,0x66,0x67,0x68,
},
{// shift-mode
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
	//	    ,   ¥,    ,    ,    ,    ,    ,    		; 0xa0(JIS Keyboard)
		  NC,0x0e,  NC,  NC,  NC,  NC,  NC,  NC,
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
	//	  F7,  F8,  F9, F10,    ,    ,    ,    		; 0xe0
		0x69,0x6a,0x6b,0x6c,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    , cap,    ,sftR		; 0xe8
		  NC,  NC,  NC,  NC,  NC,0x5d,  NC,0x70,
	//	sftL,    , ctr,    , opt,cmdR,cmdL,    		; 0xf0
		0x70,  NC,0x71,  NC,0x5b,0x5b,0x5a,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x100
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x108
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,   ↑,   ↓,   →,  ←,    ,    ,    		; 0x110
		  NC,0x3c,0x3e,0x3d,0x3b,  NC,  NC,  NC,
	//	    ,    ,  F1,  F2,  F3,  F4,  F5,  F6		; 0x118(0x11c)
		  NC,  NC,0x63,0x64,0x65,0x66,0x67,0x68,
}
};

