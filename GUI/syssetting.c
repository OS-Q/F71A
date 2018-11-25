#include "gui.h"
#include "lcd.h"
#include "text.h"
#include "touch.h"
#include "delay.h"
#include "sys.h"
#include "time.h"
#include "rtc_time.h"
#include "syssetting.h"
#include "GUI2D.h"

const u8 *SETmenu_F1[2]={"ϵͳʱ��","����У׼"};//��1����Ŀ¼Ŀ¼
const u8 *SETmenu_S1[2]={"��������","ʱ������"};//��1����Ŀ¼

static const GUI_POINT aPointUTriangle[] = {
			{-10, 0},
			{0, -15},
			{10, 0},
};

static const GUI_POINT aPointDTriangle[] = {
			{-10, 0},
			{0, 15},
			{10, 0},
};

static const GUI_POINT aPointLTriangle[] = {
			{0, -10},
			{-15, 0},
			{0, 10},
};

static const GUI_POINT aPointRTriangle[] = {
			{0, -10},
			{15, 0},
			{0, 10},
};

//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

//�·�   1  2  3  4  5  6  7  8  9  10 11 12		 
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}

void Set_time(void)
{
	struct tm time_now;
	u8  tmin,tsec,thour;
	u8  selset,lastsel;
	u8  key,arrow;
	u8  tt;

	time_now=Time_GetCalendarTime();
	tsec=time_now.tm_sec;
	tmin=time_now.tm_min;
	thour=time_now.tm_hour;
	
	Load_Gui_Bro("ȷ��");
	LCD_Fill(0,23,319,41,WHITE);
	POINT_COLOR=WHITE;	
	Show_Str(128,2,"ʱ������",16,0x01);//����ģʽ,���Զ�����
	arrow=1;
	lastsel=2;
	selset=0;
	SWAlpha=1;
	POINT_COLOR=DARKBLUE;
	for(key=0;key<4;key++)
	LCD_DrawRect(56+key,206+key,263-key,253-key);
	POINT_COLOR=BROWN;
	GL_FillPolygon(aPointLTriangle,3,45,229);
	GL_FillPolygon(aPointRTriangle,3,274,229);
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
	Pen_Point.Key_Reg=0;
	goto UPTM;

	while(1)
	{
		key=AI_Read_TP(30,10);	    
		if(key)
		{
			key=SetTime_Key_To_Num(selset);
			if(key)
			{
				if(key==1)
				{
					lastsel=selset;
					if(selset--<=0)
					selset=2;
					arrow=1;
				}
				else if(key==2)
				{
					lastsel=selset;
					if(++selset>2)
					selset=0;
					arrow=1;
				}
				else if(key==3)
				{
					switch(selset)
					{
						case 0:
							if(++thour>23) thour=0;
							break;
						case 1:
							if(++tmin>59) tmin=0;
							break;
						case 2:
							if(++tsec>59) tsec=0;
							break;
					}
				}
				else if(key==4)
				{
					switch(selset)
					{
						case 0:
							if(thour--<=0) thour=23;
							break;
						case 1:
							if(tmin--<=0) tmin=59;
							break;
						case 2:
							if(tsec--<=0) tsec=59;
							break;
					}
				}
				else if(key==5)
				{
					time_now.tm_sec=tsec;
					time_now.tm_min=tmin;
					time_now.tm_hour=thour;
					Time_SetCalendarTime(time_now);
					break;
				}
				else if(key==6)
				break;
				else{}
UPTM:				
				if(arrow)
				{
					LCD_Fill(60,210,259,249,LIGHTBLUE);
					LCD_Fill(60+selset*80,210,99+selset*80,249,BRRED);
					POINT_COLOR=WHITE;
					GL_FillPolygon(aPointUTriangle,3,79+lastsel*80,200);
					GL_FillPolygon(aPointDTriangle,3,79+lastsel*80,259);
					POINT_COLOR=BROWN;
					GL_FillPolygon(aPointUTriangle,3,79+selset*80,200);
					GL_FillPolygon(aPointDTriangle,3,79+selset*80,259);
					POINT_COLOR=WHITE;
					LCD_Show2Num(71,221,thour,16,0x01);
					LCD_DisplayChar(115,221,':',16,0x01);
					LCD_Show2Num(151,221,tmin,16,0x01);
					LCD_DisplayChar(195,221,':',16,0x01);
					LCD_Show2Num(231,221,tsec,16,0x01);
					arrow=0;
				}
				else
				{
					POINT_COLOR=WHITE;
					BACK_COLOR=BRRED;
					switch(selset)
					{
						case 0:
							LCD_Show2Num(71,221,thour,16,0x00);
							break;
						case 1:
							LCD_Show2Num(151,221,tmin,16,0x00);
							break;
						case 2:
							LCD_Show2Num(231,221,tsec,16,0x00);
							break;
					}
				}
				Delay_ms(100);			 
			}
		}
		Delay_ms(10);
		if(++tt>100)
		{
			tt=0;
			SignalLED=!SignalLED;
		}
	}
	SWAlpha=0;	
}

void Set_date(void)
{
	struct tm time_now;
	u16 tyear;
	u8  tday,tmon,tempdate;
	u8  selset,lastsel;
	u8  key,arrow;
	u8  tt;

	time_now=Time_GetCalendarTime();
	tday=time_now.tm_mday;
	tmon=time_now.tm_mon+1;
	tyear=time_now.tm_year;
	
	Load_Gui_Bro("ȷ��");
	LCD_Fill(0,23,319,41,WHITE);
	POINT_COLOR=WHITE;	
	Show_Str(128,2,"��������",16,0x01);//����ģʽ,���Զ�����
	arrow=1;
	lastsel=2;
	selset=0;
	SWAlpha=1;
	POINT_COLOR=DARKBLUE;
	for(key=0;key<4;key++)
	LCD_DrawRect(56+key,206+key,263-key,253-key);
	POINT_COLOR=BROWN;
	GL_FillPolygon(aPointLTriangle,3,45,229);
	GL_FillPolygon(aPointRTriangle,3,274,229);
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
	Pen_Point.Key_Reg=0;
	goto UPDM;

	while(1)
	{
		key=AI_Read_TP(30,10);	    
		if(key)
		{
			key=SetTime_Key_To_Num(selset);
			if(key)
			{
				if(key==1)
				{
					lastsel=selset;
					if(selset--<=0)
					selset=2;
					arrow=1;
				}
				else if(key==2)
				{
					lastsel=selset;
					if(++selset>2)
					selset=0;
					arrow=1;
				}
				else if(key==3)
				{
					switch(selset)
					{
						case 0:
							if(++tyear>2099) tyear=1970;
							break;
						case 1:
							if(++tmon>12) tmon=1;
							break;
						case 2:
							if(++tday>tempdate) tday=1;
							break;
					}
				}
				else if(key==4)
				{
					switch(selset)
					{
						case 0:
							if(--tyear<1970) tyear=2099;
							break;
						case 1:
							if(--tmon<1) tmon=12;
							break;
						case 2:
							if(--tday<1) tday=tempdate;
							break;
					}
				}
				else if(key==5)
				{
					time_now.tm_mday=tday;
					time_now.tm_mon=tmon-1;
					time_now.tm_year=tyear;
					Time_SetCalendarTime(time_now);
					break;
				}
				else if(key==6)
				break;
				else{}
UPDM:			
				if(Is_Leap_Year(tyear)&&tmon==2) tempdate=mon_table[1]+1;
				else tempdate=mon_table[tmon-1];		
				if(arrow)
				{
					LCD_Fill(60,210,259,249,LIGHTBLUE);
					LCD_Fill(60+selset*80,210,99+selset*80,249,BRRED);
					POINT_COLOR=WHITE;
					GL_FillPolygon(aPointUTriangle,3,79+lastsel*80,200);
					GL_FillPolygon(aPointDTriangle,3,79+lastsel*80,259);
					POINT_COLOR=BROWN;
					GL_FillPolygon(aPointUTriangle,3,79+selset*80,200);
					GL_FillPolygon(aPointDTriangle,3,79+selset*80,259);
					POINT_COLOR=WHITE;
					LCD_ShowNum(64,221,tyear,4,16,0x01);
					LCD_DisplayChar(112,221,'-',16,0x01);
					LCD_DisplayChar(120,221,'-',16,0x01);
					LCD_Show2Num(152,221,tmon,16,0x01);
					LCD_DisplayChar(192,221,'-',16,0x01);
					LCD_DisplayChar(200,221,'-',16,0x01);
					LCD_Show2Num(232,221,tday,16,0x01);
					arrow=0;
				}
				else
				{
					POINT_COLOR=WHITE;
					BACK_COLOR=BRRED;
					switch(selset)
					{
						case 0:
							LCD_ShowNum(64,221,tyear,4,16,0x00);
							break;
						case 1:
							LCD_Show2Num(152,221,tmon,16,0x00);
							break;
						case 2:
							LCD_Show2Num(232,221,tday,16,0x00);
							break;
					}
				}
				Delay_ms(100);			 
			}
		}
		Delay_ms(10);
		if(++tt>100)
		{
			tt=0;
			SignalLED=!SignalLED;
		}
	}
	SWAlpha=0;	
}

u8 SetTime_Key_To_Num(u8 sel)
{
	if(Is_In_Area(30,219,16,21)) return 1;//��				   
	else if(Is_In_Area(274,219,16,21)) return 2;//��
	else if(Is_In_Area(69+sel*80,185,16,21)) return 3;//��
	else if(Is_In_Area(69+sel*80,259,16,21)) return 4;//��
	else if(Is_In_Area(0,450,40,30)) return 5;//ȷ��
	else if(Is_In_Area(280,450,40,30)) return 6;//����
	else return 0;//������Ч
}

void Touch_ADJ(void)
{
	u8 tkey;
	u8 tt;
	Touch_Adjust();
	LCD_Fill(80,120,240,200,GRAY);//���հ���
	POINT_COLOR=BROWN;
	LCD_DrawRect(79,119,241,201);//���յķ���
	LCD_DrawRect(80,120,240,200);//���յķ���
	LCD_DrawRect(81,121,239,199);//���յķ���
	BACK_COLOR=GRAY;
	POINT_COLOR=RED;
	Show_Str_Mid(80,135,"����У׼�����",16,160);
	BACK_COLOR=LGRAY;
    Show_Str(110,171,"ȷ��",16,0X00);//����ģʽ,���Զ�����		   
	POINT_COLOR=BROWN;	   
	LCD_DrawRect(109,170,142,187);//���յķ���
	BACK_COLOR=LGRAY;
	POINT_COLOR=RED;
    Show_Str(170,171,"ȡ��",16,0X00);//����ģʽ,���Զ�����		   
	POINT_COLOR=BROWN;	   
	LCD_DrawRect(169,170,202,187);//���յķ���
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
	while(1)//�ȴ������˳�
	{
		tkey=AI_Read_TP(30,10);
		if(tkey)Pen_Point.Key_Reg|=0X80;//��ǰ����Ѿ������� 	   
		else if(Pen_Point.Key_Reg&0X80)//�����ɿ���
		{
			Pen_Point.Key_Reg=0;
  			if(Is_In_Area(110,171,32,16))
			{Save_Adjdata(); break;}//ȷ������������  
			if(Is_In_Area(170,171,32,16)) break;//ȡ������������
		}
		Delay_ms(10);
		if(++tt>100)
		{
			tt=0;
			SignalLED=!SignalLED;
		}	   
	}
}

void Sys_setting(void)
{
	u8 selx=0;	 
	while(1)
	{
		selx=Load_Fun_Sel(SETmenu_F1,"ϵͳ�趨","ȷ��",2,0);
		if(selx==13)return;//����
		else if(selx==0)//ѡ����ϵͳʱ��
		{
			while(1)
			{	    
				selx=Load_Fun_Sel(SETmenu_S1,"ϵͳʱ��","ȷ��",2,0);//��ʾ������ť
				if(selx==13)break;//������һ��Ŀ¼
				switch(selx)
				{
					case 0://�����趨
						Set_date();
						break;
					case 1://ʱ���趨
						Set_time();
						break;
				}
			}
		}
		else if(selx==1)
 		Touch_ADJ();
	} 

}



