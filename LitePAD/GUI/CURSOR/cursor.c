#include "cursor.h"
#include "lcd.h"  
//Mini STM32开发板 
//光标 驱动
//正点原子@ALIENTEK

//光标结构体初始化赋值
GUI_Cursor MY_Cursor=
{
	LCD_W/2, //光标当前所在的坐标
	LCD_H/2, 	
	WHITE,   //光标的颜色
//右键菜单弹出的窗口
	0,
	0,	  
	0,	          //光标状态,bit0:0,没有显示,1,开始显示了;bit1:右键菜单弹出与否
	Set_Cursor,   //单纯设置光标到某个坐标,并不清除		 
	Clear_Cursor, //单纯清除最近一次的光标		 
	Cursor_Draw,  //先清除最近一次的光标,再设置新的光标
};				 
//标准光标
//12*20大小
const u8 std_cur_tab[36]=	
{0xFF,0xFF,0x00,0x80,0x02,0x00,0x40,0x04,0x00,
 0x20,0x08,0x00,0x10,0x18,0x00,0x08,0x06,0x00,
 0x04,0x01,0x80,0x02,0x38,0x60,0x01,0x26,0x10,
 0x00,0xA1,0x90,0x00,0x60,0x60,0x00,0x20,0x00};
//标准光标掩码
const u8 std_cur_msk[36]=
{0xFF,0xFF,0x00,0xFF,0xFE,0x00,0x7F,0xFC,0x00,
 0x3F,0xF8,0x00,0x1F,0xF8,0x00,0x0F,0xFE,0x00,
 0x07,0xFF,0x80,0x03,0xFF,0xE0,0x01,0xE7,0xF0,
 0x00,0xE1,0xF0,0x00,0x60,0x60,0x00,0x20,0x00};

//光标覆盖区域数据缓存区
//12*20的光标实际上只需要125个字节
u16 cur_buf[125];   

//清除光标
//用于清除最近一次的光标
//(x,y)一般就是(MY_Cursor.X,MY_Cursor.Y)							    
void Clear_Cursor(u16 x,u16 y)
{
	u16 y0=y;
	u8 t,t1,temp,temp1;	  		 
	u8 rccnt=0;//读取到的颜色计数器	
	if((MY_Cursor.STA&0x01)==0)return;//如果光标还没开始画,则直接返回	  
    for(t=0;t<36;t++)
    {   												   
	    temp=std_cur_tab[t]; //得到数据 
		temp1=std_cur_msk[t];//得到掩码数据                         
        for(t1=0;t1<8;t1++)
		{
			if(temp1&0x80)
			{
              	POINT_COLOR=cur_buf[rccnt];//还原原来的颜色
				if(y<320)LCD_DrawPoint(x,y);//LCD_DrawPoint函数没有判断范围,这里我们需外部判断
				rccnt++;
 			}	   
			temp<<=1;
			temp1<<=1;
			y++;							   
			if((y-y0)==20)
			{
				y=y0;
				x++;
				if(x>=LCD_W)return;//X方向 越界
				break;
			}
		}  	 
	} 
}	 
//(x,y):设置光标的位置
//color:光标的颜色
//如果为白色,则为标准光标
void Set_Cursor(u16 x,u16 y)
{	
	u16 y0=y;
	u8 t,t1,temp,temp1;	  
	u16 out_color,in_color;	 
	u8 rccnt=0;//读取到的颜色计数器

	out_color=in_color=MY_Cursor.Color;
	if(MY_Cursor.Color==WHITE)out_color=BLACK;//如果为白色,则为标准光标 
	MY_Cursor.STA|=0X01;//标记开始
	MY_Cursor.X=x;//记录此次坐标
	MY_Cursor.Y=y;   
	POINT_COLOR=MY_Cursor.Color;
	BACK_COLOR=WHITE;
    for(t=0;t<36;t++)
    {   												   
	    temp=std_cur_tab[t]; //得到数据 
		temp1=std_cur_msk[t];//得到掩码数据                         
        for(t1=0;t1<8;t1++)
		{
			if(temp1&0x80)
			{
				if(temp&0x80)POINT_COLOR=out_color;//边框
                else POINT_COLOR=in_color;//内部 
				cur_buf[rccnt]=LCD_ReadPoint(x,y);//读取当前点的颜色值 
				rccnt++;
				if(y<320)LCD_DrawPoint(x,y);//LCD_DrawPoint函数没有判断范围,这里我们需外部判断
 			}//透明区域,不画出来 	   
			temp<<=1;
			temp1<<=1;
			y++;							  
			if((y-y0)==20)
			{
				y=y0;
				x++;
				if(x>=LCD_W)return;//X轴方向 越界
				break;
			}
		}  	 
	}   
}
//画光标,带清除
//该函数先清除上一次的光标
//再画下一个需要画出的光标
void Cursor_Draw(u16 x,u16 y)
{
	Clear_Cursor(MY_Cursor.X,MY_Cursor.Y);//清除上一次的光标
	Set_Cursor(x,y);
}







