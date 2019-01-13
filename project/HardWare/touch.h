#ifndef __TOUCH_H__
#define __TOUCH_H__
#include <stm32f10x_lib.h>

//Mini STM32开发板
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 驱动函数
//正点原子@ALIENTEK
//2010/6/13
//V1.3	    
//按键状态	 
#define Key_Down 0x01
#define Key_Up   0x00 
//笔杆结构体
typedef struct 
{
	//第一次按下的坐标值(LCD坐标),在AI_Read_TP函数里被记录
	//直到触点松开,该值是不会改变的
	//用来存储触点刚按下时的坐标
	u16 X0;
	u16 Y0;
	//在没有执行Convert_Pos之前是物理坐标
	//在执行了之后是LCD坐标,也就是当前坐标	    
	u16 X; //LCD坐标/当前坐标 
	u16 Y;						   	    
	u8 Key_Sta;//笔的状态
//笔寄存器
//bit7:用于标记触点被按下与否,由外部函数修改
//bit1,0:第一次按下的按键模式(1:滑动;2,点触)
	u8 Key_Reg;//笔寄存器	  
//触摸屏校准参数
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;	   
extern Pen_Holder Pen_Point;
//与触摸屏芯片连接引脚	   
#define PEN  PBin(1)   //PF7  INT
#define DOUT PBin(0)   //PF8  MISO
#define TDIN PDout(13)  //PF9  MOSI
#define TCLK PDout(3) //PF10  SCLK
#define TCS  PDout(6) //PF11 CS 
//#define BUSY PDin(12)  //PF11 BUSY
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 指令集
#define CMD_RDY 0X90  //0B10010000即用差分方式读X坐标
#define CMD_RDX	0XD0  //0B11010000即用差分方式读Y坐标   											 
#define TEMP_RD	0XF0  //0B11110000即用差分方式读Y坐标        
			  
void Touch_Init(void);		 //初始化
u8 Read_ADS(u16 *x,u16 *y);	 //带舍弃的双方向读取
u8 Read_ADS2(u16 *x,u16 *y); //带加强滤波的双方向坐标读取
u16 ADS_Read_XY(u8 xy);		 //带滤波的坐标读取(单方向)
u16 ADS_Read_AD(u8 CMD);	 //读取AD转换值					  
void ADS_Write_Byte(u8 num); //向控制芯片写入一个数据
void Draw_Touch_Point(u16 x,u16 y);//画一个坐标叫准点
void Draw_Big_Point(u16 x,u16 y);  //画一个大点
void Touch_Adjust(void);          //触摸屏校准
#ifdef ADJ_SAVE_ENABLE
void Save_Adjdata(void);		  //保存校准参数
u8 Get_Adjdata(void); 			  //读取校准参数
#endif
void Pen_Int_Set(u8 en); 		  //PEN中断使能/关闭
u8 Convert_Pos(void);           //结果转换函数
void EXTI1_IRQRoutine(void);     //pen中断函数 

u16 T_abs(u16 m,u16 n);      //求两数之差的绝对值
u8 AI_Read_TP(u8 MS_TIME,u8 interval);   //读取TP状态	 
u8 Is_In_Area(u16 x,u16 y,u16 xlen,u16 ylen);//判断按键是否在这个区域内.  	 
#endif

















