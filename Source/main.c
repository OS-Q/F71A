
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "touch.h"
#include "flash.h"
#include "vs1003.h"
#include "FAT.h"
#include "fontupd.h"
#include "text.h"
#include "sysfile.h"
#include "gui.h"
#include "picviewer.h"
#include "usbplay.h"
#include "mp3player.h"
#include "mouseplay.h"
#include "ebookplay.h"
#include "time.h"
#include "rtc_time.h"
#include "timeplay.h"
#include "ptgame.h"
#include "ov7670config.h"
#include "ov7670.h"
#include "syssetting.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Init_Periph(void);
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
#ifdef DEBUG
    debug();
#endif	  
	u8 t,key;		 
	u8 index;
	u16 detime=0;  
	
	Set_System();
	Init_Periph();
	Setup_RTC();
	SPI_Flash_Init();
	STM3210E_LCD_Init();
	FAT_Init();
	LCD_SetTextColor(Blue);
	LCD_SetBackColor(Black);
	LCD_DisplayOff();
	Delay_ms(200);
	if(LoadLOGO()) 
	LCD_DisplayStringLine(Line14, 10, "FAT System Error!");
	LCD_DisplayOn();
	Font_Init();
	Touch_Init();
	Vs1003_Init();
	Delay_ms(4000);	
	LCD_Clear(BLACK);
	while(SysInfoGet(0x00))
	{
		LCD_DisplayStringLine(Line14, 10, "FAT System Error!");
		Delay_ms(500);
		FAT_Init();
		LCD_ClearLine(Line14);
		Delay_ms(500);
	}

	LCD_DisplayOff();
	Delay_ms(200);
	Load_Sys_ICO();	 
	LCD_DisplayOn();  
	t=5;    	  
	while(1)
	{	
		key=AI_Read_TP(30,10);	    
		if(key) Pen_Point.Key_Reg|=0X80;  //标记按键已经按下	   
		else if(Pen_Point.Key_Reg&0x80)//按键松开了
		{
			Pen_Point.Key_Reg=0;		  	
			t=Touch_To_Num(0);
		} 
		else{}    						   
		if(t)
		{ 							     
			Select_Menu(0,t-1,1);//选中这个
			if(index!=t-1)Select_Menu(0,index,0);//取消上一次的
			else //此次选择和上次一样,进入该功能
			{
				switch(index)
				{
					case 0://音乐播放	    
						Mus_Viewer();	    					  						
						break; 
					case 1://浏览图片		 	  
						Pic_Viewer();//图片浏览					
						break;
					case 2://电子书阅读	  
						Ebook_Play();				
						break;
					case 3://照相机			   
						Camera_demo();  								   
						break;							
					case 4://拼图游戏	 		   
						Game_Play();						
						break; 
					case 5://鼠标画板模式
						Mouse_Play();								      
						break;     
					case 6://USB连接实验	   	    				    
						USB_Play();						
						break; 
					case 7://无线通信模式
						LCD_Clear(WHITE);
						POINT_COLOR=Blue;
						BACK_COLOR=White;
						Show_Str(60,Line10,"正在查找设备...",24,0x00);
						Delay_ms(4000);
						Show_Str(60,Line13,"未找到可用的连接",24,0x00);
						Delay_ms(1000);
						//Wireless_Play(); 							  					 
						break;
					case 8://时间模式	  
						Time_Viewer();
						break;
					case 9://文件浏览模式
						BrowseAll=0;
						if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//根目录簇号
					 	else Cur_Dir_Cluster=0;						   
						Fat_Dir_Cluster=Cur_Dir_Cluster;//父目录就是当前考
						Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发
						Load_Gui_Bro("选择");//显示GUI界面
						POINT_COLOR=WHITE;	//笔触设为白色
						Show_Str(128,2,"文件浏览",16,0x01);//叠加模式,非自动换行							
						File_Browse(T_ALLFILE);//浏览文件
						BrowseAll=1;
						break;
					case 10://计算器
						LCD_SetTextColor(Blue);
						Load_Draw_Dialog();
						SignalLED = 1;
						// Infinite loop 
						while (1)
						{
							if(Pen_Point.Key_Sta == Key_Down)
							{
								Pen_Int_Set(0);
								Pen_Point.Key_Sta = Key_Up;
								Convert_Pos();
								if((Pen_Point.X<32) && (Pen_Point.Y<16))
								break;
								do
								{
									Convert_Pos();
									if((Pen_Point.X>296) && (Pen_Point.Y<16))
									Load_Draw_Dialog();
									else
									Draw_Big_Point(Pen_Point.X, Pen_Point.Y);
								}while(PEN == 0);
								Pen_Int_Set(1);
							}
						}
						Pen_Int_Set(1);
						//Calculator_demo();
						break;
					case 11://系统设定
						Sys_setting();
						break;
				}
			Load_Sys_ICO();//重新加载系统菜单 
			Select_Menu(0,index,1);//选中上次选中的
			Pen_Point.Key_Reg=0;
			Pen_Point.Key_Sta=Key_Up;		
			}
			index=t-1; 									   
			t=0;	  
		}else 
		{
			Delay_ms(10);
			detime++;
			if(detime<10)SignalLED=0;//2s亮100ms
			else 
			{
				SignalLED=1;
				if(detime>200)detime=0;
			}										   
		}				   	   
	}			   	

}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.	  
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  printf("Wrong parameters value: file %s on line %d\r\n", file, line)
  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/*************外设初始化***************/
void Init_Periph(void)
{
	SysTick_Config();
	Init_USART1(115200);
}	

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
