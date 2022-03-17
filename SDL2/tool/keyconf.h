#ifndef _x68_keyconf
#define _x68_keyconf

extern int32_t hWndKeyConf;
extern uint16_t  KeyConf_CodeW;
extern uint32_t KeyConf_CodeL;
extern int8_t KeyConfMessage[255];
LRESULT CALLBACK KeyConfProc(int32_t hWnd, uint32_t msg, uint16_t wp, uint32_t lp);

#endif // _x68_keyconf
