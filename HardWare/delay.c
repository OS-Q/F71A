#include "delay.h"


//SUPER STM32ZE������
//SQ�ı�ҵ���--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//��ȷ��ʱ������ͨ��systickʵ��
//2011/3/17


static vu32 TimingDelay = 0;


/*****ϵͳ�δ�ʱ������******/
//systickʱ��Դʹ��ϵͳʱ��HCLK
//����systick 1us�ж�һ��
//ʹ��systick�ж�����
void SysTick_Config(void)
{
  /* Configure HCLK clock as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
 
  /* SysTick interrupt each 100 Hz with HCLK equal to 72MHz */
  SysTick_SetReload(72);

  /* Enable the SysTick Interrupt */
  SysTick_ITConfig(ENABLE);
}


//systick�жϵ��ú���
void Decrement_TimingDelay(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


//΢�뼶��ʱ����
void Delay_us(u32 nus)
{
  TimingDelay = nus;

  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
  
  while(TimingDelay != 0)
  {
  }

  /* Disable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Disable);

  /* Clear the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Clear);
}


//���뼶��ʱ����
void Delay_ms(u32 nms)
{
  TimingDelay = nms*1000;

  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
  
  while(TimingDelay != 0)
  {
  }

  /* Disable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Disable);

  /* Clear the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Clear);
}


































