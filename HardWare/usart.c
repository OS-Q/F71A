#include "usart.h"

//SUPER STM32ZE������
//SQ�ı�ҵ���--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//2011/3/18


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//��USART1��������
	USART_SendData(USART1, ch); 
	//ѭ������,ֱ��������� 
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

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[64];     //���ջ���,���64���ֽ�.
//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
u8 USART_RX_STA=0;       //����״̬���	  
  
void USART1_IRQRoutine(void)
{
	u8 res;	    
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)//���յ�����
	{	 
		res = USART_ReceiveData(USART1); 
		if((USART_RX_STA & 0x80) == 0)//����δ���
		{
			if((USART_RX_STA & 0x40) != 0)//���յ���0x0d
			{
				if(res != 0x0a)
				USART_RX_STA = 0;   //���մ���,���¿�ʼ
				else 
				USART_RX_STA |= 0x80;	  //��������� 
				//ռλ������һ������жϣ�������ܵ�������
			}
			else //��û�յ�0X0D
			{	
				if(res == 0x0d)	   //�յ�0x0d
				USART_RX_STA|=0x40;		//��λ���λ
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3F] = res;
					USART_RX_STA++;
					if(USART_RX_STA > 63)
					USART_RX_STA=0;     //�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	}  											 
} 
#endif

										 
//��ʼ��IO ����1
//bound:������
//CHECK OK
void Init_USART1(u32 bound)
{  	 
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
#if EN_USART1_RX
	NVIC_InitTypeDef NVIC_InitStructure;
#endif
	//����USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//����GPIOAʱ��
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

	//USART1ģ������
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
	//���ܻ���ǿ��ж�ʹ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  	
	//��ʼ��USART1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	//�����ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	//�ж�ͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//��ʼ��USART1
	NVIC_Init(&NVIC_InitStructure);	 
#endif
	//ʹ��USART1
	USART_Cmd(USART1, ENABLE); 
}


