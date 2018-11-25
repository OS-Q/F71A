#ifndef __PTGAME_H
#define __PTGAME_H	
#include <stm32f10x_lib.h>
   
//Mini STM32开发板
//RTC实时时钟 驱动代码			 
//正点原子@ALIENTEK
//2010/6/6
void Game_Play(void);
void PTGame_Play(void);
void PTG_Best_Show(void);
void PTG_Win_Msg(void);
u8 PTGame_Key_To_Num(void);   
#endif







