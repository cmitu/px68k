#ifndef winx68k_statusbar_h
#define winx68k_statusbar_h

#ifdef __cplusplus
extern "C" {
#endif

#if 1

#define	StatBar_Show(s)
#define	StatBar_UpdateTimer()
#define	StatBar_SetFDD(d,f)
#define	StatBar_ParamFDD(d,a,i,b)
#define	StatBar_HDD(s)

#else

void StatBar_Redraw(void);
void StatBar_Show(int32_t sw);
void StatBar_Draw(DRAWITEMSTRUCT* dis);
void StatBar_FDName(int32_t drv, char* name);
void StatBar_FDD(int32_t drv, int32_t led, int32_t col);
void StatBar_UpdateTimer(void);
void StatBar_SetFDD(int32_t drv, char* file);
void StatBar_ParamFDD(int32_t drv, int32_t access, int32_t insert, int32_t blink);
void StatBar_HDD(int32_t sw);

#endif

#ifdef __cplusplus
};
#endif

#endif //winx68k_statusbar_h
