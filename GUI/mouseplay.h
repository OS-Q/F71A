#ifndef __MOUSEPLAY_H
#define __MOUSEPLAY_H
#include <stm32f10x_lib.h>
#define LeftButtonBit	0x01
#define RightButtonBit	0x02
#define MiddleButtonBit	0x04
#define LState	0x01
#define RState	0x02
#define MState	0x04
#define XState	0x08
#define YState	0x10
#define WState	0x20
	    
void Mouse_Play(void);
void Joystick_Send(u8 *report);

#endif


