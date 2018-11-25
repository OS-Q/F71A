#include "mouseplay.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "text.h"
#include "delay.h"  
#include "touch.h"	   	   	     	  	    
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"

	 
#define MINMOV 1
#define CURSOR_STEP 1  
						 
//鼠标画板功能演示函数   
void Mouse_Play(void)
{				  	
	u8 key;
	u16 t;
	u8 Mouse_Buffer[4] = {0, 0, 0, 0};
  	s8 X = 0, Y = 0;
//	s8 W = 0;
	u8 state;
	LCD_Clear(WHITE);	
	Pen_Int_Set(0);  //关闭中断  
	//USB配置
	USB_Disconnect_Config();
	/* MAL configuration */
	Set_USBClock();
	USB_Interrupts_Config();
	USB_MouseInit();	
	while (bDeviceState != CONFIGURED);

	POINT_COLOR=DARKBLUE;//深蓝色	 
	LCD_DrawLine(0,457,319,457);
 	LCD_Fill(0,458,319,467,LIGHTBLUE);//填充底部颜色(浅蓝色)	
 	LCD_Fill(0,468,319,479,GRAYBLUE); //填充底部颜色(灰蓝色)	  	   
	POINT_COLOR=WHITE;//白色字体
	Show_Str(283,460,"返回",16,0x01);  //叠加模式,非自动换行 
	Show_Str(5,460,"USB设备连接成功。",16,0x01);
	Pen_Point.Key_Reg = 0;
	Pen_Point.Key_Sta = Key_Up;
	Pen_Int_Set(1);  //打开中断
	while(1)
	{
		Delay_ms(5);
		key=AI_Read_TP(30,5);
				    
		if(key==2) 
		{
			if(Is_In_Area(280,456,40,24))//点动按下
			continue;
			*Mouse_Buffer |= LeftButtonBit;
			state |= LState;
		}		   
		else if(Pen_Point.Key_Reg&0x02)
		{
			if(Is_In_Area(280,450,40,30))//点动按下
			{
				if(PowerOff() == USB_SUCCESS) break;													
				else continue;
			}
			Pen_Point.Key_Reg &= 0xFD;
			*Mouse_Buffer &= ~LeftButtonBit;
			state |= LState;
		}
		else{}
		 
		if(key==1)
		{
			if((T_abs(Pen_Point.X,Pen_Point.X0)>MINMOV)&&(T_abs(Pen_Point.Y,Pen_Point.Y0)>MINMOV))
			{
				X = Pen_Point.X;
				Y = Pen_Point.Y;
				X -= Pen_Point.X0;
				Y -= Pen_Point.Y0;
				Pen_Point.X0 = Pen_Point.X;
				Pen_Point.Y0 = Pen_Point.Y;
				state |= XState|YState;
				Mouse_Buffer[1] = X*CURSOR_STEP;
  				Mouse_Buffer[2] = Y*CURSOR_STEP;	
			}
		}
		else if(Pen_Point.Key_Reg&0x01)
		Pen_Point.Key_Reg &= 0xFE;
  		else{}
		if(state)
		{
			Joystick_Send(Mouse_Buffer);
			state = 0;
		}
		Delay_ms(10);
		t++;
		if(t>400) {t = 0; SignalLED = 0;}
		if(t==40) SignalLED = 1;		
	}
	Delay_ms(500);
}


/*******************************************************************************
* Function Name : Joystick_Send.
* Description   : prepares buffer to be sent containing Joystick event infos.
* Input         : Keys: keys received from terminal.
* Output        : None.
* Return value  : None.
*******************************************************************************/
void Joystick_Send(u8 *report)
{
  /*copy mouse position info in ENDP1 Tx Packet Memory Area*/
  UserToPMABufferCopy(report, GetEPTxAddr(ENDP1), 4);
  /* enable endpoint for transmission */
  SetEPTxValid(ENDP1);
}

