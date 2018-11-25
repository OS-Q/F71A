#include "delay.h"
#include "GUI2D.h"
#include "jpegbmp.h"
#include "lcd.h"
#include "text.h"
#include "touch.h"
#include "gui.h"
#include "time.h"
#include "rtc_time.h"
#include "timeplay.h"
#include "sysfile.h"
#include "sys.h"
#include "temp.h"

const u8 dis_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,//0, 1, 2, 3, 4, 5
    0x82,0xf8,0x80,0x90,0x3F,0x06,0xff};//6, 7, 8, 9, -, C, off 

static const GUI_POINT aPointSMGH[] = {
			{0, 0},
			{3, -3},
			{32, -3},
			{35, 0},
			{32, 3},
			{3, 3},
			{0, 0},
};
/*
static const GUI_POINT aPointSMGHS[] = {
			{1, 0},
			{4, 0},
			{4, 2},
			{1, 2},
};
*/
static const GUI_POINT aPointSMGV[] = {
			{0, 0},
			{-3, 3},
			{-3, 32},
			{0, 35},
			{3, 32},
			{3, 3},
			{0, 0},
};
			

static const GUI_POINT aPointHPin[] = {
/*			{-2, 0},
			{-2, -18},
			{-7, -18},
			{-4, -40},
			{0, -64},
			{4, -40},
			{7, -18},
			{2, -18},
			{2, 0},*/
			{-6, 0},
			{-6, -40},
			{0, -64},
			{6, -40},
			{6, 0},		
};
static GUI_POINT aPointHPinRotate[5];
/*static const GUI_POINT aPointHPinFrame[] = {
			{-4, 0},
			{-3, -63},
			{3, -63},
			{4, 0}		
};
static GUI_POINT aPointHPinFrameRotate[4];*/

static const GUI_POINT aPointMPin[] = {
/*			{-2, 0},
			{-2, -18},
			{-6, -18},
			{-3, -50},
			{0, -84},
			{3, -50},
			{6, -18},
			{2, -18},
			{2, 0},*/
			{-5, 0},
			{-5, -50},
			{0, -84},
			{5, -50},
			{5, 0},		
};
static GUI_POINT aPointMPinRotate[5]; 
/*static const GUI_POINT aPointMPinFrame[] = {
			{-3, 0},
			{-2, -83},
			{2, -83},
			{3, 0}		
};
static GUI_POINT aPointMPinFrameRotate[4];*/

static const GUI_POINT aPointSPin[] = {
			{-2, 32},
			{-2, -102},
			{2, -102},
			{2, 32},		
};
static GUI_POINT aPointSPinRotate[4]; 
/*static const GUI_POINT aPointSPinFrame[] = {
			{-3, 0},
			{-2, -83},
			{2, -83},
			{3, 0}		
};
static GUI_POINT aPointSPinFrameRotate[4];*/
u8 TimeDisplay;	 

const u8 *week_table[7]={"星期天","星期一","星期二","星期三","星期四","星期五","星期六"};
//时间显示模式
void Time_Viewer(void)
{
	struct tm time_now;
	FileInfoStruct temp;
	u8 key;
	u8 curSec,curMin,curHour,curDay;
	u8 redraw,redis_date;
	float angleSec,angleMin,angleHour;
	int temperature; 
					   
    LCD_Clear(0x5AEB);//清屏
	Temp_Init();
	BACK_COLOR=WHITE; 
	temp.F_Type=T_BMP;//文件属性设置
	temp.F_StartCluster=sys_ico[12]; 
	AI_LoadPicFile(&temp,0,0,319,319);

	TimeDisplay=0;
	redraw=0;redis_date=0;
	time_now=Time_GetCalendarTime();
	curSec=time_now.tm_sec;
	curMin=time_now.tm_min;
	curHour=time_now.tm_hour;
	angleSec=curSec*0.1047197;
	angleMin=curMin*0.1047197+angleSec/60;
	angleHour=curHour*0.5235988+angleMin/12;
	RTC_ClearITPendingBit(RTC_IT_SEC);
	RTC_ITConfig(RTC_IT_SEC,ENABLE);
	goto M;	
	while(1)
	{
		if(TimeDisplay)
		{
			TimeDisplay=0;
			SignalLED=!SignalLED;
			time_now=Time_GetCalendarTime();
			curSec=time_now.tm_sec;
			angleSec=curSec*0.1047197;
			if(curMin!=time_now.tm_min)
			{
				curMin=time_now.tm_min;
				if(curHour!=time_now.tm_hour)
				{
					curHour=time_now.tm_hour;
					if(curDay!=time_now.tm_mday)
					{curDay=time_now.tm_mday;redis_date=1;}
				}

			}
			if(curSec%10==0)
			{
				angleMin=curMin*0.1047197+angleSec/60;
				if(curMin%2==0)
				angleHour=curHour*0.5235988+angleMin/12;
				redraw=1;
			}

			if(curSec%4==1)
			{
				temperature=Get_Temperature();
				if(temperature<0)
				{
					SMG_display(10,350,10);
					temperature=-temperature;
				}
				else
				SMG_display(10,350,12);
				SMG_ShowNum(65,350,temperature/10,3);
				POINT_COLOR=0xFFE0;
				LCD_FillCircle(240,430,3);
				SMG_display(260,350,temperature%10);	
			}

/*			angleSec=curSec*6;
			POINT_COLOR=0X001F;		
			LCD_Show2Num(200,400,curSec,16,0x00);
			LCD_Show3Num(250,400,angleSec,16,0x00);*/								
	
			tempReshow=(u16 *)(iclip+2730);
			tempOff=0;
			DrawFuc=0;
			GL_FillPolygon(aPointSPinRotate, 4, 160, 160);

			if(redraw)
			{
				tempReshow=(u16 *)(iclip+1365);
				tempOff=0;
				DrawFuc=0;
				GL_FillPolygon(aPointMPinRotate, 5, 160, 160);
				tempReshow=(u16 *)iclip;
				tempOff=0;
				DrawFuc=0;
				GL_FillPolygon(aPointHPinRotate, 5, 160, 160);

				if(redis_date)
				{
M:					POINT_COLOR=0x001F;
					BACK_COLOR=0xEF7D;
					LCD_Show2Num(120,197,time_now.tm_year/100,16,0x00);
					LCD_Show2Num(136,197,time_now.tm_year%100,16,0x00);
					LCD_DisplayChar(152,197,'-',16,0x00);
					LCD_Show2Num(160,197,time_now.tm_mon+1,16,0x00);
					LCD_DisplayChar(176,197,'-',16,0x00);
					LCD_Show2Num(184,197,time_now.tm_mday,16,0x00);
					BACK_COLOR=0xFFFF;
					Show_Str(124,115,(u8 *)week_table[time_now.tm_wday],24,0x00);
					redis_date=0;
				}
							
				GUI_RotatePolygon(aPointHPinRotate,aPointHPin,5,angleHour);
//				GUI_RotatePolygon(aPointHPinFrameRotate,aPointHPinFrame,4,angleHour);
				GUI_RotatePolygon(aPointMPinRotate,aPointMPin,5,angleMin);
//				GUI_RotatePolygon(aPointMPinFrameRotate,aPointMPinFrame,4,angleMin);
				tempReshow=(u16 *)iclip;
				tempOff=0;
				DrawFuc=1;
				PinAlpha=600;
				POINT_COLOR=0x033F;
				GL_FillPolygon(aPointHPinRotate, 5, 160, 160);
//				POINT_COLOR=0x0000;
//				GL_DrawPolygonM(aPointHPinFrameRotate, 4, 160, 160);
				tempReshow=(u16 *)(iclip+1365);
				tempOff=0;
				DrawFuc=1;
				PinAlpha=800;
				POINT_COLOR=0x033F;
				GL_FillPolygon(aPointMPinRotate, 5, 160, 160);
//				POINT_COLOR=0x0000;
//				GL_DrawPolygonM(aPointMPinFrameRotate, 4, 160, 160);
				redraw=0;
			}
			
			GUI_RotatePolygon(aPointSPinRotate,aPointSPin,4,angleSec);
			tempReshow=(u16 *)(iclip+2730);
			tempOff=0;
			DrawFuc=1;
			PinAlpha=900;
			POINT_COLOR=0x033F;
			GL_FillPolygon(aPointSPinRotate, 4, 160, 160);

			POINT_COLOR=0x001F;
			LCD_FillCircle(160, 160, 7);
			POINT_COLOR=0x632C;
			LCD_FillCircle(160, 160, 3);
		}
		key=AI_Read_TP(30,10);
		if(key)
		if(Is_In_Area(280,440,40,40))
		break;
		Delay_ms(10);
	}
	RTC_ITConfig(RTC_IT_SEC,DISABLE);
	RTC_ClearITPendingBit(RTC_IT_SEC);
	Temp_Close();
}

void SMG_display(u16 sx, u16 sy, u8 num)
{
	u8 i;
	s16 Xoff,Yoff;
	num=dis_code[num];
	SWAlpha=1;
	for(i=0;i<8;i++)
	{
		if(num&0x01)
		POINT_COLOR=0x632C;
		else
		POINT_COLOR=0xFFE0;
		num>>=1;
		switch(i)
		{
			case 0:
				Xoff=sx+5;
				Yoff=sy+3;
				break;	
			case 1:
				Xoff=sx+42;
				Yoff=sy+5;
				break;
			case 2:
				Xoff=sx+42;
				Yoff=sy+44;
				break;
			case 3:
				Xoff=sx+5;
				Yoff=sy+81;
				break;
			case 4:
				Xoff=sx+3;
				Yoff=sy+44;
				break;
			case 5:
				Xoff=sx+3;
				Yoff=sy+5;
				break;
			case 6:
				Xoff=sx+5;
				Yoff=sy+42;
				break;
			case 7:
				Xoff=sx+54;
				Yoff=sy+80;
				break;
			default:break;
		}
		if(i%3==0)
		GL_FillPolygon(aPointSMGH, 7, Xoff, Yoff);
		else if(i==7) break;
		else
		GL_FillPolygon(aPointSMGV, 7, Xoff, Yoff);
	}
	SWAlpha=0;
}

void SMG_ShowNum(u16 x,u16 y,u16 num,u8 len)
{         	
	u8 t,temp;
	u8 enshow = 0;						   
	for(t = 0;t < len;t++)
	{
		temp = (num/mypow(10, len-t-1))%10;
		if(enshow==0 && t<(len-1))
		{
			if(temp == 0)
			{
				SMG_display(x, y, 12);
				x += 55;
				continue;
			}
			else 
			enshow = 1; 		 	 
		}
	 	SMG_display(x, y, temp); 
		x += 55;
	}
} 

