#include "delay.h"
#include "lcd.h"
#include "sys.h"
#include "text.h"
#include "touch.h"
#include "SCCB.h"
#include "ov7670config.h"
#include "ov7670.h"

////////////////////////////
//功能：写OV7660寄存器
//返回：1-成功	0-失败
u8 wrOV7670Reg(u8 regID, u8 regDat)
{
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	Delay_us(10);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	Delay_us(10);
  	if(0==SCCBwriteByte(regDat))
	{
		stopSCCB();
		return(0);
	}
  	stopSCCB();
	
  	return(1);
}
////////////////////////////
//功能：读OV7660寄存器
//返回：1-成功	0-失败
u8 rdOV7670Reg(u8 regID, u8 *regDat)
{
	//通过写操作设置寄存器地址
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	Delay_us(10);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	stopSCCB();
	
	Delay_us(10);
	
	//设置寄存器地址后，才是读
	startSCCB();
	if(0==SCCBwriteByte(0x43))
	{
		stopSCCB();
		return(0);
	}
	Delay_us(10);
  	*regDat=SCCBreadByte();
  	noAck();
  	stopSCCB();
  	return(1);
}

/*
 * Write a list of register settings; ff/ff stops the process.
 */
u8 OV7670_write_array(const regval_list *vals)
{
	u8 ret;
	while (vals->reg_num != 0xff || vals->value != 0xff) {
		ret = wrOV7670Reg(vals->reg_num, vals->value);
		if (ret == 0) return 0;			
		vals++;
	}
	return 1;
}

u8 OV7670_reset(void)
{
	return wrOV7670Reg(REG_COM7, COM7_RESET);
}		 
			   
//(140,16,640,480) is good for VGA
//(272,16,320,240) is good for QVGA
u8 OV7670_set_hw(u16 hstart, u16 hstop, u16 vstart, u16 vstop)		
{
	u8 ret = 1;
	u8 v;
/*
 * Horizontal: 11 bits, top 8 live in hstart and hstop.  Bottom 3 of
 * hstart are in href[2:0], bottom 3 of hstop in href[5:3].  There is
 * a mystery "edge offset" value in the top two bits of href.
 */
	ret &=  wrOV7670Reg(REG_HSTART, (hstart >> 3) & 0xff);
	ret &= wrOV7670Reg(REG_HSTOP, (hstop >> 3) & 0xff);
	ret &= rdOV7670Reg(REG_HREF, &v);
	v = (v & 0xc0) | ((hstop & 0x7) << 3) | (hstart & 0x7);
	ret &= wrOV7670Reg(REG_HREF, v);
/*
 * Vertical: similar arrangement, but only 10 bits.
 */
	ret &= wrOV7670Reg(REG_VSTART, (vstart >> 2) & 0xff);
	ret &= wrOV7670Reg(REG_VSTOP, (vstop >> 2) & 0xff);
	ret &= rdOV7670Reg(REG_VREF, &v);
	v = (v & 0xf0) | ((vstop & 0x3) << 2) | (vstart & 0x3);
	ret &= wrOV7670Reg(REG_VREF, v);
	return ret;
}

/* OV7670_init() */
//返回1成功，返回0失败
u8 OV7670_Init(void)
{	
	u8 i;
    u8 temp;

	OV7670_InitIO();	
	InitSCCB();//io init..

	rdOV7670Reg(0x0a, &i);
	rdOV7670Reg(0x0a, &i);
	if(0==OV7670_reset()) return 0;//Reset SCCB	
	Delay_ms(50);	
	if(0==rdOV7670Reg(0x0b, &temp)) return 0;//读ID
		
    if(temp==0x73)//OV7670
	{
	   if(0==OV7670_write_array(ov7670_default_regs1))
	   return 0;
//	   if(0==OV7670_write_array(ov7670_fmt_rgb565))
//	   return 0;	  
	}
    else if(temp==0x21)//OV7725
	{
	   if(0==OV7670_write_array(ov7725_default_regs))
	   return 0;	
	}

	if(0==wrOV7670Reg(0x15, 0x10)) return 0;	
//	if(0==OV7670_set_hw(478,10,14,490))	return 0;
	
	Show_Str(32,Line6,"摄像头初始化正常！",16,0);
	if(temp==0x73)
	Show_Str(32,Line8,"摄像头型号是：OV7670。",16,0);
	else if(temp==0x21)
	Show_Str(32,Line8,"摄像头型号是：OV7725。",16,0);
	else
	;
	Delay_ms(1000);
	OV7670_InitInt();
	return 1; //ok		
}

void VSYNC_IRQRoutine(void)
{
	u32 i=76800;
	u16 temp;
	while(EXTI_GetFlagStatus(EXTI_Line13)==RESET);
    AL422WRST = 0; 
    AL422WEN = 1;
    AL422WEN = 1;
    AL422WRST = 1;
	EXTI_ClearFlag(EXTI_Line13);
   	while(EXTI_GetFlagStatus(EXTI_Line13)==RESET);
    AL422WEN = 0;
	AL422REN = 0;

	LCD_SetDisplayWindow(0, 0, 319, 239);
	AL422RRST = 0; 
    AL422RCK = 0;
    AL422RCK = 1;
    AL422RRST = 1;
//    AL422RCK = 0;    
//    AL422RCK = 1;
		
	while(i--)
	{
		AL422RCK = 0;
		i++;i--;
		temp = AL422Data;
		AL422RCK = 1;
		i = i;
		AL422RCK = 0;
		i++;i--;
		temp |= AL422Data<<8;	
		AL422RCK =1;
		LCD->LCD_RAM = temp;	
	}
	AL422REN = 1;
	EXTI_ClearFlag(EXTI_Line13);
}

void Camera_demo(void)
{
	u8 key;
	LCD_Clear(WHITE);
	POINT_COLOR=BLUE;
	BACK_COLOR=WHITE;
	if(OV7670_Init())
	SignalLED=1;	  	
	else
	{
	   Show_Str(32,Line6,"摄像头初始化失败！",16,0);
	   SignalLED=1;
	   Delay_ms(2000);
	   RCC_MCOConfig(RCC_MCO_NoClock);
	   return;
	}
	EXTI_ClearFlag(EXTI_Line13);
	while(1)
	{
		VSYNC_IRQRoutine();
		key=AI_Read_TP(30,20);
		if(key)
		if(Is_In_Area(280,460,40,20))
		break;
	}
	RCC_MCOConfig(RCC_MCO_NoClock);
}




void OV7670_InitIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    //使能GPIOA,GPIOB,GPIOC,GPIOE时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
						   RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);

	//AL422DataPin 上拉输入
    GPIO_InitStructure.GPIO_Pin = AL422DataPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(AL422DataPort, &GPIO_InitStructure);

	//OV7670VSYNCPin 下拉输入
    GPIO_InitStructure.GPIO_Pin = OV7670VSYNCPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670VSYNCPort, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AL422WENPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(AL422WENPort, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AL422WRSTPin | AL422RENPin | AL422RRSTPin;
	GPIO_Init(AL422RENPort, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AL422RCKPin;
	GPIO_Init(AL422RCKPort, &GPIO_InitStructure);
	
	//MCO输出配置 备用推挽
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_MCOConfig(RCC_MCO_HSE);
}

void OV7670_InitInt(void)
{
//	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);                         
    GPIO_EXTILineConfig(OV7670VSYNCEXTIPort, OV7670VSYNCEXTIPin);

	//VSYNC 外部中断线设置
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    //EXTI_Mode参数：EXTI_Mode_Interrupt、EXTI_Mode_Event
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //EXTI_Trigger参数：EXTI_Trigger_Rising、EXTI_Trigger_Falling
    //、EXTI_Trigger_Rising_Falling
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
				 
/*    //初始化EXTI13中断通道
	NVIC_InitStructure.NVIC_IRQChannel = EXTI13_IRQChannel;
	//抢占优先级 3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	//子优先级 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	//中断通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化EXTI0
	NVIC_Init(&NVIC_InitStructure);*/	
}

