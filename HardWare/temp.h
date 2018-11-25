#ifndef _TEMP_H
#define _TEMP_H
#include <stm32f10x_lib.h>


void Temp_Init(void);
int Get_Temperature(void);
void Temp_Close(void);

#endif

