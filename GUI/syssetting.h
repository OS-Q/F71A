#ifndef _SYS_SETTING_H
#define _SYS_SETTING_H
#include <stm32f10x_lib.h>

u8 Is_Leap_Year(u16 year);
void Set_time(void);
void Set_date(void);
u8 SetTime_Key_To_Num(u8 sel);
void Touch_ADJ(void);
void Sys_setting(void);

#endif

