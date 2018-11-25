#include "usbplay.h"			   
#include "lcd.h"
#include "text.h"			 
#include "delay.h"  
#include "touch.h"	   	   	     	  	    
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "sdcard.h"
					   

//bit0:��ʾ����������SD��д������
//bit1:��ʾ��������SD����������
//bit2:SD��д���ݴ����־λ
//bit3:SD�������ݴ����־λ
//bit4:1,��ʾ��������ѯ����(�������ӻ�������)
u8 Usb_Status_Reg=0; 
//����USB����   
void USB_Play(void) 
{	  	 
	u8 offline=0;
    u8 USB_STA=0;
    u8 werr=0,rerr=0;
	u8 i,key; 
						 	     
	LCD_Clear(LGRAY);//����ɫ 
	LCD_SetTextColor(Blue); 
	LCD_SetBackColor(LGRAY);
	Pen_Int_Set(0);  //�ر��ж�  
	//USB����
	USB_Disconnect_Config();
	/* MAL configuration */
	Set_USBClock();
	MAL_Config();
	USB_Interrupts_Config();
	USB_MassInit();	
	while (bDeviceState != CONFIGURED);
	Show_Str(64, Line10, "USB�豸���ӳɹ���", 16, 0);
	Show_Str(80, Line12, "����ͨ����...", 16, 0);
	Delay_ms(1000);
	while (1)
	{
		if(USB_STA != Usb_Status_Reg)
		{
			if(Usb_Status_Reg&0x01)	   //д
			{
				LCD_DisplayStringLine(Line16, 8, "USB Writing...");
				Usb_Status_Reg &= 0xFE;
			}
			if(Usb_Status_Reg&0x02)	   //��
			{
				LCD_DisplayStringLine(Line16, 8, "USB Reading...");
				Usb_Status_Reg &= 0xFD;
			}
			if(Usb_Status_Reg&0x04)	   //д����
			{
				LCD_SetTextColor(Red);
				werr++;
				LCD_DisplayStringLine(Line18, 8, "USB Write Err:");
				LCD_ShowNum(176, 288, werr, 3, 16, 0x00);
				LCD_SetTextColor(Blue);
			}
			if(Usb_Status_Reg&0x08)	   //������
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
	Pen_Int_Set(1); //�����ж�
	POINT_COLOR=DARKBLUE;//����ɫ	 
	LCD_DrawLine(0,457,319,457);
 	LCD_Fill(0,458,319,467,LIGHTBLUE);//���ײ���ɫ(ǳ��ɫ)	
 	LCD_Fill(0,468,319,479,GRAYBLUE); //���ײ���ɫ(����ɫ)	  	   
	POINT_COLOR=WHITE;//��ɫ����
	Show_Str(5,460,"USB�豸�����ѶϿ���",16,0x01);   //����ģʽ,���Զ�����		  
	Show_Str(283,460,"����",16,0x01);  //����ģʽ,���Զ����� 
	while(1)
	{	
		key=AI_Read_TP(30,10);	    
		if(key) Pen_Point.Key_Reg|=0X80;  //��ǰ����Ѿ�����	   
		else if(Pen_Point.Key_Reg&0x80)//�����ɿ���
		{
			if((Pen_Point.Key_Reg&0x02)&&Is_In_Area(280,450,40,30))//�㶯����
			if(PowerOff() == USB_SUCCESS)													
			break;
			Pen_Point.Key_Reg=0;
		}
		else{}
		Delay_ms(10);
	}
	SignalLED=1;		 	 
}
