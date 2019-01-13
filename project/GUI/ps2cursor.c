#include "ps2cursor.h"
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
#include "cursor.h"


u16 tcnt=0;
void Mouse_Play(void)
{				  	
	u8 t;
	short x,y;
	u16 x0,y0;
	u16 temp_color=BLUE;		 

	x=y=0;
	LCD_Clear(WHITE);
	POINT_COLOR=BLUE;
	BACK_COLOR=WHITE;

RST:t=Init_Mouse();//初始化鼠标
	if(t==0)LCD_ShowString(60,130,"Mouse Init OK!");
	else
	{
		printf("ERROR_CODE:%d\n",t);
		LCD_ShowString(60,130,"Error CODE:");
		LCD_ShowNum(148,130,t,3,16);//填充模式
		LED0=!LED0;
		goto RST;
	}
	GUI_Cursor_Init();//初始化光标
	while(1)
	{
		if(PS2_Status&0x80)//得到了一次数据
		{		  			 
			Mouse_Data_Pro();
			x+=MouseX.x_pos;
			if(x<0)x=0;	 
			if(x>239)x=239;
			y+=MouseX.y_pos;
			if(y<0)y=0;
			if(y>319)y=319;				   
/////////////////////////////////////////////////////////////////////			
			Clear_Cursor(MY_Cursor.X,MY_Cursor.Y);//清除上一次的光标
			if(MouseX.bt_mask)//有按键按下
			{
				if(MY_Cursor.STA&0x02)//右键菜单已经弹出了
				{
					if(x>=(MY_Cursor.m_x0+65)||y>=(MY_Cursor.m_y0+29)||x<=MY_Cursor.m_x0||y<=MY_Cursor.m_y0)//不在菜单内
					{
						MY_Cursor.STA&=~0x02;//清除弹出标志
						Reshow_Temp_Color(MY_Cursor.m_x0,MY_Cursor.m_y0,65,29);//恢复之前的颜色
					}else //在菜单内
					{
						if(MouseX.bt_mask&LEFT_DOWN)//左键按下
						{
							temp_color=LCD_ReadPoint(x,y);//读取该点的颜色
							if(temp_color!=FRAME2)//不是按在边框上
							{
								POINT_COLOR=temp_color;//修改画笔颜色
								MY_Cursor.STA&=~0x02;//清除弹出标志
								Reshow_Temp_Color(MY_Cursor.m_x0,MY_Cursor.m_y0,65,29);//恢复之前的颜色	   
							}
							MouseX.bt_mask&=~(0x01);//此次不执行画图
						}
					}
				}
				if((MouseX.bt_mask&LEFT_DOWN)&&((MY_Cursor.STA&0x02)==0))//左键按下,并且没有弹出窗口
				{
					POINT_COLOR=temp_color;
					MY_Cursor.Color=temp_color;
					Draw_Big_Point(x,y);  
				}	  
				if((MouseX.bt_mask&MID_DOWN)&&((MY_Cursor.STA&0x02)==0))//擦除,没有弹出窗口
				{
					POINT_COLOR=WHITE;
					MY_Cursor.Color=RED;
					Draw_Big_Point(x,y); 
				}	 
				if(MouseX.bt_mask&RIGHT_DOWN)//右键
				{
			     	if((MY_Cursor.STA&0x02)==0)//右键菜单还未弹出
					{
						MY_Cursor.STA|=0x02;//
						x0=x;
						y0=y;
						if(x0>(240-65-1))//靠右边了
						{
							x0-=65;//左边显示
							if(y0>(320-29-1))y0-=29;//上边显示  
						}else if(y0>(320-29-1))y0-=29;//上边显示  
						MY_Cursor.m_x0=x0;	//记录菜单所在的
						MY_Cursor.m_y0=y0;
						Read_Temp_Color(x0,y0,65,29);//先读出之前的颜色值
						Draw_Color_Table(x0,y0);
					}
				} 		 
			}else MY_Cursor.Color=WHITE;//没有按键按下	  
			Set_Cursor(x,y);
/////////////////////////////////////////////////////////////////////
			PS2_Status=MOUSE;
			PS2_En_Data_Report();//使能数据报告 
		}else if(PS2_Status&0x40)
		{			 
			PS2_Status=MOUSE;   //读数错误处理
			printf("ERROR\n");
			while(Init_Mouse());//重新初始化鼠标
		}
		t++;		  
		if(t==200)
		{
			t=0;
			LED0=!LED0;
		}
	} 		   
}


