#ifndef __DELAY_H
#define __DELAY_H 			   
#include <stm32f10x_lib.h>

//SUPER STM32ZE������
//SQ�ı�ҵ���--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//���롢΢�뼶��ʱ����������
//2011/3/17

void SysTick_Config(void);
void Decrement_TimingDelay(void);
void Delay_us(u32 nus);
void Delay_ms(u32 nms);

#endif





























