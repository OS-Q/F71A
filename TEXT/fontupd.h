#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include <stm32f10x_lib.h>	 
//Mini STM32开发板
//中文汉字支持程序 V1.1					   
//正点原子@ALIENTEK
//2010/5/23	

#define EN_UPDATE_FONT //使能字体更新,通过关闭这里实现禁止字库更新
//#define SYSTEM_UPDATE   //更新全部flash内容，将执行全片擦除

//flash存储空间分配，按扇区划分，EN25F16共有2MB空间
//每扇区4KB，共有512个扇区，扇区号0-511
//0-9扇区保留给系统使用
//0扇区存储字库存在信息
//1扇区存储触摸校准信息 
#define FONT16ADDR   0x00A000	   //10-73扇区存储16*16点阵GB2312字库
#define FONT12ADDR   0x04A000	   //74-109扇区存储12*12点阵GB2312字库
#define FONT24ADDR   0x06E000	   //110-253扇区存储24*124点阵GB2312字库
#define UNI2GBKADDR	 0x0FE000	   //254-264扇区存储unicode转GBK表
#define GBK2UNIADDR  0x109000	   //265-276扇区存储GBK转unicode表（0x115000）

u8 Update_Font(void);//更新字库   
u8 Font_Init(void);//初始化字库
//void DMA_M2MInit(void);
//void DMA_M2MStart(u32 SRCAddr, u32 DSTAddr, u32 num);
#endif





















