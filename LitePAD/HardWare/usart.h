#ifndef __USART_H
#define __USART_H
#include <stm32f10x_lib.h>
#include "stdio.h"
	 
//SUPER STM32ZE������
//SQ�ı�ҵ���--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//���нӿڳ�ʼ������������
//2011/3/18


#if EN_USART1_RX	  	
extern u8 USART_RX_BUF[64];     //���ջ���,���63���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART_RX_STA;         //����״̬���	

void USART1_IRQRoutine(void);
#endif

void Init_USART1(u32 bound);

#endif	   
















