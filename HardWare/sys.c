#include "sys.h"

//SUPER STM32ZE������
//SQ�ı�ҵ���--Digit PDA
//QQ:291681964  EMAIL:sq871115@yahoo.cn
//ϵͳʱ�ӳ�ʼ�����ж�ϵͳ��ʼ����GPIO��ʼ��
//2011/3/17

ErrorStatus HSEStartUpStatus = SUCCESS;


/*****��ʼ��ϵͳʱ��********/
//ϵͳʱ�Ӳ����ⲿ����������PLL��Ƶ��72M
//ʹ��ϵͳʹ�õ�����ʱ��
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


/******�ں��ж�ϵͳ��ʼ��*******/
//�����ж����ȼ���
//�����ж�������
void InterruptConfig(void)
{
  NVIC_DeInit();

  /*- NVIC_PriorityGroup_0: 0 bits for pre-emption priority 4 bits for subpriority
    - NVIC_PriorityGroup_1: 1 bits for pre-emption priority	3 bits for subpriority
    - NVIC_PriorityGroup_2: 2 bits for pre-emption priority 2 bits for subpriority                    
    - NVIC_PriorityGroup_3: 3 bits for pre-emption priority 1 bits for subpriority                  
    - NVIC_PriorityGroup_4: 4 bits for pre-emption priority 0 bits for subpriority*/                     
  //�ж����ȼ������ã�2λ��ռ���ȼ���2λ�����ȼ�                     
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  //�ж�����������
  //ͨ���궨�������ж�������
  #if VECT_TAB_RAM != 0
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
  #else   
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  #endif 
}


/*******�ⲿ�ж�EXTI����********/
void Init_EXTI(void)
{
//  //����EXTI��ʼ���ṹ����
//  EXTI_InitTypeDef EXTI_InitStructure;
//  //����EXTI�ж������Ӷ˿�
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
//  //��λEXTI���Ĵ���ΪĬ��ֵ
//  EXTI_DeInit();
//  //����EXTI_InitStructure����ԱΪĬ��ֵ
//  EXIT_StructInit(&EXTI_InitStructure);
//
//  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//  //EXTI_Mode������EXTI_Mode_Interrupt��EXTI_Mode_Event
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  //EXTI_Trigger������EXTI_Trigger_Rising��EXTI_Trigger_Falling
//  //��EXTI_Trigger_Rising_Falling
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
}


/*********GPIO����**********/
//��ʼ��board�Ͽ��ء�LED�������ӵ�GPIO������������
void Init_HwGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //��λGPIOx���Ĵ���
  //ע��˺�����رո�GPIO��ʱ��
  //GPIO_DeInit(GPIOA);
  //ʹ��GPIOFʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  //װ��GPIO_InitStructure����ԱĬ�ϳ�ʼֵ
  GPIO_StructInit(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  //GPIO_Speed������GPIO_Speed_2MHz��GPIO_Speed_10MHz��GPIO_Speed_50MHz
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  //GPIO_Mode������GPIO_Mode_AIN��GPIO_Mode_IN_FlOATING��GPIO_Mode_IPU
  //��GPIO_Mode_IPD��GPIO_Mode_Out_PP��GPIO_Mode_Out_OD��GPIO_Mode_AF_PP
  //��GPIO_Mode_AF_OD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  SignalLED = 1;
}


/******����ϵͳ���л���*******/
void Set_System(void)
{
  Init_SysClock();
  InterruptConfig();
  Init_EXTI();
  Init_HwGPIO();
}



                         
                     
                        
  
  
