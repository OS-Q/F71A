#include "delay.h"


//SUPER STM32ZE开发板
//SQ的毕业设计--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//精确延时函数，通过systick实现
//2011/3/17


static vu32 TimingDelay = 0;


/*****系统滴答定时器配置******/
//systick时钟源使用系统时钟HCLK
//配置systick 1us中断一次
//使能systick中断请求
void SysTick_Config(void)
{
  /* Configure HCLK clock as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
 
  /* SysTick interrupt each 100 Hz with HCLK equal to 72MHz */
  SysTick_SetReload(72);

  /* Enable the SysTick Interrupt */
  SysTick_ITConfig(ENABLE);
}


//systick中断调用函数
void Decrement_TimingDelay(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


//微秒级延时函数
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


//毫秒级延时函数
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


































