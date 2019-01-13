#include "touchplay.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "fat.h"
#include "sysfile.h"
#include "jpegbmp.h"
#include "usart.h"	 
#include "led.h"
#include "mouse.h"	     
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK
						 
//触摸画板功能演示函数   
void Touch_Play(void)
{				  	
	u8 i;		 
	u8 key;
	u8 M_STA=0;
	u16 temp_color;
	LCD_Clear(WHITE);	    
	Load_Gui_Show("重画","颜色","返回");//加载界面  
	POINT_COLOR=RED;  
	while(1)
	{
 		key=AI_Read_TP(20);//得到触点的状态
		if(key)//触摸屏被按下						  
		{	
			Pen_Point.Key_Reg|=0x80;//标记按下了按键

			if(Pen_Point.Y<296&&M_STA==0)//在画图区域内,并且颜色框没有弹出来
			{
				Draw_Big_Point(Pen_Point.X,Pen_Point.Y);//画点	      
			}      
		}else if(Pen_Point.Key_Reg&0X80)//按键松开了 
		{
			if(Pen_Point.Key_Reg&0x02)//点动按下
			{
				if(M_STA)//菜单已经弹出来了
				{
					if(Pen_Point.X>88&&Pen_Point.X<153&&Pen_Point.Y>268)//在颜色框内
					{
						temp_color=LCD_ReadPoint(Pen_Point.X,Pen_Point.Y);//读取该点的颜色
						if(temp_color!=FRAME2)//不是按在边框上
						{
							Reshow_Temp_Color(88,268,65,29);//恢复之前的颜色
							POINT_COLOR=temp_color;//修改画笔颜色	    
							M_STA=0;	   
						}   
					}else 
					{
						Reshow_Temp_Color(88,268,65,29);//恢复之前的颜色
						M_STA=0;
					}
				}
				if(Is_In_Area(5,300,32,16))//清除按钮按下了
				{
					LCD_Fill(0,0,239,296,WHITE);//清屏
				}else if(Is_In_Area(104,300,32,16))//颜色按钮按下了
				{
					Read_Temp_Color(88,268,65,29);//先读出之前的颜色值
					Draw_Color_Table(88,268); 
					M_STA=1;//标记颜色框已经显示出来了 
				}else if(Is_In_Area(203,300,32,16))//返回按钮按下了
				{
					Pen_Point.Key_Reg=0;//清除笔状态			   
					return;
				}   
			}
			Pen_Point.Key_Reg=0;//清除笔状态	  	 
		}else delay_ms(1);
		i++;
		if(i==200)
		{
			i=0;
			LED0=!LED0;
		}	
	} 		   
}


