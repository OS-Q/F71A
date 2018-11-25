
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
		if(key) Pen_Point.Key_Reg|=0X80;  //��ǰ����Ѿ�����	   
		else if(Pen_Point.Key_Reg&0x80)//�����ɿ���
		{
			Pen_Point.Key_Reg=0;		  	
			t=Touch_To_Num(0);
		} 
		else{}    						   
		if(t)
		{ 							     
			Select_Menu(0,t-1,1);//ѡ�����
			if(index!=t-1)Select_Menu(0,index,0);//ȡ����һ�ε�
			else //�˴�ѡ����ϴ�һ��,����ù���
			{
				switch(index)
				{
					case 0://���ֲ���	    
						Mus_Viewer();	    					  						
						break; 
					case 1://���ͼƬ		 	  
						Pic_Viewer();//ͼƬ���					
						break;
					case 2://�������Ķ�	  
						Ebook_Play();				
						break;
					case 3://�����			   
						Camera_demo();  								   
						break;							
					case 4://ƴͼ��Ϸ	 		   
						Game_Play();						
						break; 
					case 5://��껭��ģʽ
						Mouse_Play();								      
						break;     
					case 6://USB����ʵ��	   	    				    
						USB_Play();						
						break; 
					case 7://����ͨ��ģʽ
						LCD_Clear(WHITE);
						POINT_COLOR=Blue;
						BACK_COLOR=White;
						Show_Str(60,Line10,"���ڲ����豸...",24,0x00);
						Delay_ms(4000);
						Show_Str(60,Line13,"δ�ҵ����õ�����",24,0x00);
						Delay_ms(1000);
						//Wireless_Play(); 							  					 
						break;
					case 8://ʱ��ģʽ	  
						Time_Viewer();
						break;
					case 9://�ļ����ģʽ
						BrowseAll=0;
						if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//��Ŀ¼�غ�
					 	else Cur_Dir_Cluster=0;						   
						Fat_Dir_Cluster=Cur_Dir_Cluster;//��Ŀ¼���ǵ�ǰ��
						Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
						Load_Gui_Bro("ѡ��");//��ʾGUI����
						POINT_COLOR=WHITE;	//�ʴ���Ϊ��ɫ
						Show_Str(128,2,"�ļ����",16,0x01);//����ģʽ,���Զ�����							
						File_Browse(T_ALLFILE);//����ļ�
						BrowseAll=1;
						break;
					case 10://������
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
					case 11://ϵͳ�趨
						Sys_setting();
						break;
				}
			Load_Sys_ICO();//���¼���ϵͳ�˵� 
			Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
			Pen_Point.Key_Reg=0;
			Pen_Point.Key_Sta=Key_Up;		
			}
			index=t-1; 									   
			t=0;	  
		}else 
		{
			Delay_ms(10);
			detime++;
			if(detime<10)SignalLED=0;//2s��100ms
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


/*************�����ʼ��***************/
void Init_Periph(void)
{
	SysTick_Config();
	Init_USART1(115200);
}	

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
