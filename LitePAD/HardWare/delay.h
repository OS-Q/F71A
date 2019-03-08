#ifndef __DELAY_H
#define __DELAY_H 			   
#include <stm32f10x_lib.h>

//SUPER STM32ZE开发板
//SQ的毕业设计--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//毫秒、微秒级延时，函数声明
//2011/3/17

void SysTick_Config(void);
void Decrement_TimingDelay(void);
void Delay_us(u32 nus);
void Delay_ms(u32 nms);

#endif





























