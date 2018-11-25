#ifndef __CURSOR_H
#define __CURSOR_H
#include "sys.h"
//光标
typedef struct 
{
	u16 X;	  //光标当前所在的坐标
	u16 Y;		
	u16 Color;//光标的颜色
//右键菜单弹出的窗口
	u16 m_x0;
	u16 m_y0;	  
	u8 STA;	     //光标状态,bit0:0,没有显示,1,开始显示了;bit1:右键菜单弹出与否
	void(*Set_Cur)(u16,u16); //单纯设置光标到某个坐标,并不清除		 
	void(*Clr_Cur)(u16,u16); //单纯清除最近一次的光标		 
	void(*Cursor)(u16,u16);	 //先清除最近一次的光标,再设置新的光标	 
}GUI_Cursor;  

extern GUI_Cursor MY_Cursor;				 
void Set_Cursor(u16 x,u16 y);  //设置光标
void Clear_Cursor(u16 x,u16 y);//清除光标
void Cursor_Draw(u16 x,u16 y);//画光标,带清除	 
#endif











