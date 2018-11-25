#ifndef __USART_H
#define __USART_H
#include <stm32f10x_lib.h>
#include "stdio.h"
	 
//SUPER STM32ZE开发板
//SQ的毕业设计--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//串行接口初始化、声明函数
//2011/3/18


#if EN_USART1_RX	  	
extern u8 USART_RX_BUF[64];     //接收缓冲,最大63个字节.末字节为换行符 
extern u8 USART_RX_STA;         //接收状态标记	

void USART1_IRQRoutine(void);
#endif

void Init_USART1(u32 bound);

#endif	   
















