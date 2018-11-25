#include "sys.h"

//SUPER STM32ZE开发板
//SQ的毕业设计--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//系统时钟初始化、中断系统初始化、GPIO初始化
//2011/3/17

ErrorStatus HSEStartUpStatus = SUCCESS;


/*****初始化系统时钟********/
//系统时钟采用外部晶体振荡器，PLL倍频至72M
//使能系统使用的外设时钟
void Init_SysClock(void)
{
  ErrorStatus HSEStartUpStatus = SUCCESS;
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_Bypass);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
 
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

	/* ADCCLK = PCLK2/6 = 12 MHz*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
 
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);
 
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
 
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
 
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
 
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}


/******内核中断系统初始化*******/
//配置中断优先级组
//配置中断向量表
void InterruptConfig(void)
{
  NVIC_DeInit();

  /*- NVIC_PriorityGroup_0: 0 bits for pre-emption priority 4 bits for subpriority
    - NVIC_PriorityGroup_1: 1 bits for pre-emption priority	3 bits for subpriority
    - NVIC_PriorityGroup_2: 2 bits for pre-emption priority 2 bits for subpriority                    
    - NVIC_PriorityGroup_3: 3 bits for pre-emption priority 1 bits for subpriority                  
    - NVIC_PriorityGroup_4: 4 bits for pre-emption priority 0 bits for subpriority*/                     
  //中断优先级组配置：2位抢占优先级，2位子优先级                     
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  //中断向量表配置
  //通过宏定义配置中断向量表
  #if VECT_TAB_RAM != 0
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
  #else   
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  #endif 
}


/*******外部中断EXTI配置********/
void Init_EXTI(void)
{
//  //声明EXTI初始化结构类型
//  EXTI_InitTypeDef EXTI_InitStructure;
//  //配置EXTI中断线连接端口
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
//  //复位EXTI各寄存器为默认值
//  EXTI_DeInit();
//  //填入EXTI_InitStructure各成员为默认值
//  EXIT_StructInit(&EXTI_InitStructure);
//
//  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//  //EXTI_Mode参数：EXTI_Mode_Interrupt、EXTI_Mode_Event
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  //EXTI_Trigger参数：EXTI_Trigger_Rising、EXTI_Trigger_Falling
//  //、EXTI_Trigger_Rising_Falling
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
}


/*********GPIO配置**********/
//初始化board上开关、LED等所连接的GPIO，不包括外设
void Init_HwGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //复位GPIOx各寄存器
  //注意此函数会关闭该GPIO的时钟
  //GPIO_DeInit(GPIOA);
  //使能GPIOF时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  //装入GPIO_InitStructure各成员默认初始值
  GPIO_StructInit(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  //GPIO_Speed参数：GPIO_Speed_2MHz、GPIO_Speed_10MHz、GPIO_Speed_50MHz
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  //GPIO_Mode参数：GPIO_Mode_AIN、GPIO_Mode_IN_FlOATING、GPIO_Mode_IPU
  //、GPIO_Mode_IPD、GPIO_Mode_Out_PP、GPIO_Mode_Out_OD、GPIO_Mode_AF_PP
  //、GPIO_Mode_AF_OD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  SignalLED = 1;
}


/******配置系统运行环境*******/
void Set_System(void)
{
  Init_SysClock();
  InterruptConfig();
  Init_EXTI();
  Init_HwGPIO();
}



                         
                     
                        
  
  
