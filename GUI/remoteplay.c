#include "remoteplay.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"  
#include "touch.h"	   	   	     	  
#include "usart.h"	 
#include "led.h"   
#include "key.h"    
#include "remote.h"
					   
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK

void Remote_Play(void) 
{	  	 	 
	u8 i,key;
						 	    
	Load_Gui_Show("","","返回");//加载界面 
	LCD_Fill(0,0,240,296,LGRAY);//填充灰色 
	POINT_COLOR=BLUE;  
	BACK_COLOR=LGRAY;  
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"Remote TEST");	
	LCD_ShowString(60,90,"ALIENTEK");
		 						   	 
	LCD_ShowString(30,130, "KEYVAL:");
	LCD_ShowString(130,130,"KEYCNT:");	  
	LCD_ShowString(30,150, "SYMBOL:");
	POINT_COLOR=RED;  
	KEY_Init();//先初始化按键 
	Remote_Init();//红外初始化 	    			     	 		 
	while(1)
	{	 
 		key=KEY_Scan();//得到触点的状态
		if(key==2)//返回键被按下						  
		{	
			Ex_NVIC_Config(GPIO_C,1,FTIR);//将line1映射到PC.1，下降沿触发.  
			return;
		} 			 	 		  
		i++;
		if(i==200)//200ms时间到了
		{
			i=0;
			LED0=!LED0;//提示系统在运行	  "          "
			if(LED0)LCD_ShowString(60,110,"Standby...");
	 		else LCD_ShowString(60,110,"          ");
		}
		if(Remote_Rdy)
		{
			key=Remote_Process();
			LCD_ShowNum(86,130,key,3,16);//显示键值
			LCD_ShowNum(186,130,Remote_Cnt,3,16);//显示按键次数
			switch(key)
			{
				case 0:
					LCD_ShowString(86,150, "ERROR");
					break;
				case 162://按下此键，退出
					Ex_NVIC_Config(GPIO_C,1,FTIR);//将line1映射到PC.1，下降沿触发.
					return;	 
				case 98:
					LCD_ShowString(86,150, "UP    ");
					break;
				case 2:
					LCD_ShowString(86,150, "PLAY  ");
					break;
				case 226:
					LCD_ShowString(86,150, "ALIEN ");
					break;
				case 194:
					LCD_ShowString(86,150, "RIGHT ");
					break;
				case 34:
					LCD_ShowString(86,150, "LEFT  ");
					break;
				case 56:
					LCD_ShowString(86,150, "8     ");
					break;
				case 224:
					LCD_ShowString(86,150, "VOL-  ");
					break;
				case 168:
					LCD_ShowString(86,150, "DOWN  ");
					break;
				case 144:
					LCD_ShowString(86,150, "VOL+  ");
					break;
				case 104:
					LCD_ShowString(86,150, "1     ");
					break;
				case 152:
					LCD_ShowString(86,150, "2     ");
					break;
				case 176:
					LCD_ShowString(86,150, "3     ");
					break;
				case 48:
					LCD_ShowString(86,150, "4     ");
					break;
				case 24:
					LCD_ShowString(86,150, "5     ");
					break;
				case 122:
					LCD_ShowString(86,150, "6     ");
					break;
				case 16:
					LCD_ShowString(86,150, "7     ");
					break;
				case 90:
					LCD_ShowString(86,150, "9     ");
					break;
				case 66:
					LCD_ShowString(86,150, "0     ");
					break;
				case 82:
					LCD_ShowString(86,150, "DELETE");
					break;
			}
		}else delay_ms(1);	  
	} 	   		  
} 



