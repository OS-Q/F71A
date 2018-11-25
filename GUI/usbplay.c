#include "usbplay.h"			   
#include "lcd.h"
#include "text.h"			 
#include "delay.h"  
#include "touch.h"	   	   	     	  	    
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "sdcard.h"
					   

//bit0:表示电脑正在向SD卡写入数据
//bit1:表示电脑正从SD卡读出数据
//bit2:SD卡写数据错误标志位
//bit3:SD卡读数据错误标志位
//bit4:1,表示电脑有轮询操作(表明连接还保持着)
u8 Usb_Status_Reg=0; 
//进入USB连接   
void USB_Play(void) 
{	  	 
	u8 offline=0;
    u8 USB_STA=0;
    u8 werr=0,rerr=0;
	u8 i,key; 
						 	     
	LCD_Clear(LGRAY);//填充灰色 
	LCD_SetTextColor(Blue); 
	LCD_SetBackColor(LGRAY);
	Pen_Int_Set(0);  //关闭中断  
	//USB配置
	USB_Disconnect_Config();
	/* MAL configuration */
	Set_USBClock();
	MAL_Config();
	USB_Interrupts_Config();
	USB_MassInit();	
	while (bDeviceState != CONFIGURED);
	Show_Str(64, Line10, "USB设备连接成功。", 16, 0);
	Show_Str(80, Line12, "正在通信中...", 16, 0);
	Delay_ms(1000);
	while (1)
	{
		if(USB_STA != Usb_Status_Reg)
		{
			if(Usb_Status_Reg&0x01)	   //写
			{
				LCD_DisplayStringLine(Line16, 8, "USB Writing...");
				Usb_Status_Reg &= 0xFE;
			}
			if(Usb_Status_Reg&0x02)	   //读
			{
				LCD_DisplayStringLine(Line16, 8, "USB Reading...");
				Usb_Status_Reg &= 0xFD;
			}
			if(Usb_Status_Reg&0x04)	   //写错误
			{
				LCD_SetTextColor(Red);
				werr++;
				LCD_DisplayStringLine(Line18, 8, "USB Write Err:");
				LCD_ShowNum(176, 288, werr, 3, 16, 0x00);
				LCD_SetTextColor(Blue);
			}
			if(Usb_Status_Reg&0x08)	   //读错误
			{
				LCD_SetTextColor(Red);
				rerr++;
				LCD_DisplayStringLine(Line20, 8, "USB Read Err:");
				LCD_ShowNum(168, 320, rerr, 3, 16, 0x00);
				LCD_SetTextColor(Blue);
			}
			USB_STA = Usb_Status_Reg;
		}
		Delay_ms(1);
		i++;
		if(i >= 200)
		{
			i = 0;
			if(Usb_Status_Reg&0x10)
			{
				offline = 0;
				bDeviceState = CONFIGURED;
			}
			else
			{
				offline++;
				if(offline > 20)
				{
					bDeviceState=UNCONNECTED;
					break;
				}
			}
			Usb_Status_Reg = 0;
		}
	}
	Pen_Int_Set(1); //开启中断
	POINT_COLOR=DARKBLUE;//深蓝色	 
	LCD_DrawLine(0,457,319,457);
 	LCD_Fill(0,458,319,467,LIGHTBLUE);//填充底部颜色(浅蓝色)	
 	LCD_Fill(0,468,319,479,GRAYBLUE); //填充底部颜色(灰蓝色)	  	   
	POINT_COLOR=WHITE;//白色字体
	Show_Str(5,460,"USB设备连接已断开。",16,0x01);   //叠加模式,非自动换行		  
	Show_Str(283,460,"返回",16,0x01);  //叠加模式,非自动换行 
	while(1)
	{	
		key=AI_Read_TP(30,10);	    
		if(key) Pen_Point.Key_Reg|=0X80;  //标记按键已经按下	   
		else if(Pen_Point.Key_Reg&0x80)//按键松开了
		{
			if((Pen_Point.Key_Reg&0x02)&&Is_In_Area(280,450,40,30))//点动按下
			if(PowerOff() == USB_SUCCESS)													
			break;
			Pen_Point.Key_Reg=0;
		}
		else{}
		Delay_ms(10);
	}
	SignalLED=1;		 	 
}
