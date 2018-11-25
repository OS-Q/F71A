#include "wirelessplay.h"
#include "24l01.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"		   
#include "usart.h"	 
#include "led.h"    
#include "key.h"
#include "spi.h"	     
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK

//设置24L01的模式,TX/RX
//返回值:2,发送模式
//       1,接收模式
u8 Wireless_Mode_Set(void)
{
	u8 key,t=0;	   
	LCD_Clear(LGRAY);					  
	NRF24L01_Init();    //初始化NRF24L01 
	BACK_COLOR=LGRAY;//设置字体为蓝色   	   
	POINT_COLOR=BLUE;//设置字体为蓝色    	  			 
	while(NRF24L01_Check())//检测不到24L01
	{
 		POINT_COLOR=RED;//设置字体为红色	   
		Show_Str(60,130,"24L01检测失败",16,0);
		delay_ms(500);
		Show_Str(60,130,"请检查       ",16,0);
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
		t++;
		if(t>5)return 0;//失败了
	}				  
	LCD_Fill(60,130,240,160,LGRAY);//清除显示	     
	Load_Gui_Show("","","返回");//加载界面 
	LCD_Fill(0,0,240,296,LGRAY);//填充灰色 
	GUI_Draw_Button(40,60,160,40,"设置为发送模式",BLACK);
 	GUI_Draw_Button(40,140,160,40,"设置为接收模式",BLACK);	   
	while(1)//等待模式选择
	{
		key=AI_Read_TP(20);//得到触点的状态
		if(key)//触摸屏被按下						  
		{	
			Pen_Point.Key_Reg|=0x80;//标记按下了按键   
		}else if(Pen_Point.Key_Reg&0X80)//按键松开了 
		{
			if(Pen_Point.Key_Reg&0x03)//不论点动还是滑动,都可以触发
			{	    
                if(Is_In_Area(203,300,32,16))//返回按钮按下了
				{
					Pen_Point.Key_Reg=0;//清除笔状态			   
					return 0;//直接退出
				}
                if(Is_In_Area(40,60,160,40))//发送模式
				{
					Pen_Point.Key_Reg=0;//清除笔状态			   
					return 2; 
				} 
                if(Is_In_Area(40,140,160,40))//接收模式
				{
					Pen_Point.Key_Reg=0;//清除笔状态			   
					return 1;//直接退出
				}    
			}
			Pen_Point.Key_Reg=0;//清除笔状态	  	 
		}else delay_ms(1);	
		t++;
		if(t==200)
		{
			LED0=!LED0;
			t=0;
		}				   
	}  	
}
						 
//触摸画板功能演示函数   
void Wireless_Play(void)
{				  	
	u8 err=0,i;		 
	u8 key;	 
	u8 mode;//24L01的模式设置
	u8 tmp_buf[5];//低4个字节,传递坐标值,最高一个字节:0,正常画图,1清除;2,退出;  
	  
	mode=Wireless_Mode_Set();
	if(mode==0)
	{
		SPIx_Init();//设置回原来的				 
		SPIx_SetSpeed(SPI_SPEED_4);//18Mhz 
		return;//24L01检测失败,直接退出
	}	 			
	if(mode==2)
	{			    
		Load_Gui_Show("清除","发送","返回");//加载界面 
		TX_Mode();//发送模式
	}else
	{
		Load_Gui_Show("清除","接收","返回");//加载界面 
		RX_Mode();//接收模式
	}
	LCD_Fill(0,0,240,296,LGRAY);//填充灰色 
	POINT_COLOR=BLUE;//设置字体为蓝色  	 
	while(1)
	{	  		   				 
		key=AI_Read_TP(20);//得到触点的状态
		if(mode==1)//接收模式
		{
			if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
			{
				Pen_Point.X=tmp_buf[0];
				Pen_Point.X=(Pen_Point.X<<8)+tmp_buf[1];
 				Pen_Point.Y=tmp_buf[2];
				Pen_Point.Y=(Pen_Point.Y<<8)+tmp_buf[3];
				switch(tmp_buf[4])
				{
					case 0://正常画点
						Draw_Big_Point(Pen_Point.X,Pen_Point.Y);//画点
						break;
					case 1://清除
						LCD_Fill(0,0,240,296,LGRAY);//填充灰色
						break;
					case 2://退出
						Pen_Point.Key_Reg=0;//清除笔状态
						SPIx_Init();//设置回原来的				 
						SPIx_SetSpeed(SPI_SPEED_4);//18Mhz 
						return; 
				}	  
			}
		}
		if(key)//触摸屏被按下						  
		{	
			Pen_Point.Key_Reg|=0x80;//标记按下了按键    
			if(Pen_Point.Y<296&&mode==2)//在画图区域内,且为发送模式
			{
				tmp_buf[0]=Pen_Point.X>>8;
				tmp_buf[1]=Pen_Point.X&0xFF;
				tmp_buf[2]=Pen_Point.Y>>8;	 
				tmp_buf[3]=Pen_Point.Y&0xFF; 
				tmp_buf[4]=0;//功能为0,正常画图  
				Draw_Big_Point(Pen_Point.X,Pen_Point.Y);//画点
				err=0;	      
				while(NRF24L01_TxPacket(tmp_buf)!=TX_OK)//等待发送完成
				{
					err++;
					if(err>10)return;//连续超过10次错误,则返回
				}
			}      
		}else if(Pen_Point.Key_Reg&0X80)//按键松开了 
		{
			if(Pen_Point.Key_Reg&0x02)//点动按下
			{	    
                if(Is_In_Area(203,300,32,16))//返回按钮按下了
				{
					if(mode==2)
					{
						tmp_buf[4]=2;//功能为2,退出
						NRF24L01_TxPacket(tmp_buf);//发送一次
					} 
					Pen_Point.Key_Reg=0;//清除笔状态
					SPIx_Init();//设置回原来的				 
					SPIx_SetSpeed(SPI_SPEED_4);//18Mhz 			   
					return;
				}
				if(Is_In_Area(5,300,32,16))//清除按钮按下了
				{
					if(mode==2)
					{
						tmp_buf[4]=1;//功能为1,清屏
						NRF24L01_TxPacket(tmp_buf);//发送一次
					} 									   	  
					LCD_Fill(0,0,239,296,WHITE);//清屏
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






