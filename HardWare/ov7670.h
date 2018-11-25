#ifndef _OV7670_H
#define _OV7670_H

#include <stm32f10x_lib.h>

#define CHANGE_REG_NUM 178
#define CHANGE_REG_NUM1 72

#define GPIO_High8Bit     ((u16)0xFF00)
#define GPIO_Low8Bit      ((u16)0x00FF)   
#define AL422DataPort    GPIOC
#define AL422DataPin     GPIO_Low8Bit
#define AL422Data        (((u16)(GPIOC->IDR))&0x00FF)

#define OV7670VSYNCPort	     GPIOC
#define OV7670VSYNCPin	     GPIO_Pin_13
#define OV7670VSYNCEXTIPort  GPIO_PortSourceGPIOC
#define OV7670VSYNCEXTIPin   GPIO_PinSource13  

#define AL422WENPort	GPIOB
#define AL422WENPin	    GPIO_Pin_9
#define AL422WEN        PBout(9)

#define AL422WRSTPort	GPIOE
#define AL422WRSTPin	GPIO_Pin_4
#define AL422WRST       PEout(4)


#define AL422RENPort	GPIOE
#define AL422RENPin	    GPIO_Pin_5
#define AL422REN        PEout(5)

#define AL422RRSTPort	GPIOE
#define AL422RRSTPin	GPIO_Pin_6
#define AL422RRST       PEout(6)

#define AL422RCKPort	GPIOA
#define AL422RCKPin	    GPIO_Pin_0
#define AL422RCK        PAout(0)

u8 wrOV7670Reg(u8 regID, u8 regDat);
u8 rdOV7670Reg(u8 regID, u8 *regDat);
u8 OV7670_write_array(const regval_list *vals);
u8 OV7670_reset(void);
u8 OV7670_set_hw(u16 hstart, u16 hstop, u16 vstart, u16 vstop);
void OV7670_InitIO(void);
void OV7670_InitInt(void);
u8 OV7670_Init(void);
void VSYNC_IRQRoutine(void);
void Camera_demo(void);

#endif /* _OV7660_H */

