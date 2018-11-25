#include "usart.h"

//SUPER STM32ZE开发板
//SQ的毕业设计--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//2011/3/18


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//从USART1发送数据
	USART_SendData(USART1, ch); 
	//循环发送,直到发送完毕 
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);   
	return ch;
}
/*******************************************************************************
* Function Name  : PUTCHAR_PROTOTYPE
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//PUTCHAR_PROTOTYPE
//{
//  /* Write a character to the USART */
//  USART_SendData(USART1, (u8) ch);
//
//  /* Loop until the end of transmission */
//  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
//  {
//  }
//
//  return ch;
//}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
u8 USART_RX_STA=0;       //接收状态标记	  
  
void USART1_IRQRoutine(void)
{
	u8 res;	    
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)//接收到数据
	{	 
		res = USART_ReceiveData(USART1); 
		if((USART_RX_STA & 0x80) == 0)//接收未完成
		{
			if((USART_RX_STA & 0x40) != 0)//接收到了0x0d
			{
				if(res != 0x0a)
				USART_RX_STA = 0;   //接收错误,重新开始
				else 
				USART_RX_STA |= 0x80;	  //接收完成了 
				//占位，触发一次软件中断，处理接受到的数据
			}
			else //还没收到0X0D
			{	
				if(res == 0x0d)	   //收到0x0d
				USART_RX_STA|=0x40;		//置位标记位
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3F] = res;
					USART_RX_STA++;
					if(USART_RX_STA > 63)
					USART_RX_STA=0;     //接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	}  											 
} 
#endif

										 
//初始化IO 串口1
//bound:波特率
//CHECK OK
void Init_USART1(u32 bound)
{  	 
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
#if EN_USART1_RX
	NVIC_InitTypeDef NVIC_InitStructure;
#endif
	//启动USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//启动GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//USART1_TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1模块配置
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
#if EN_USART1_RX	
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
#else
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
#endif   //EN_USART1_RX
	USART_Init(USART1, &USART_InitStructure);

#if EN_USART1_RX
	//接受缓冲非空中断使能
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  	
	//初始化USART1中断通道
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	//子优先级 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	//中断通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化USART1
	NVIC_Init(&NVIC_InitStructure);	 
#endif
	//使能USART1
	USART_Cmd(USART1, ENABLE); 
}


