#ifndef __TOUCH_H__
#define __TOUCH_H__
#include <stm32f10x_lib.h>

//Mini STM32������
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ��������
//����ԭ��@ALIENTEK
//2010/6/13
//V1.3	    
//����״̬	 
#define Key_Down 0x01
#define Key_Up   0x00 
//�ʸ˽ṹ��
typedef struct 
{
	//��һ�ΰ��µ�����ֵ(LCD����),��AI_Read_TP�����ﱻ��¼
	//ֱ�������ɿ�,��ֵ�ǲ���ı��
	//�����洢����հ���ʱ������
	u16 X0;
	u16 Y0;
	//��û��ִ��Convert_Pos֮ǰ����������
	//��ִ����֮����LCD����,Ҳ���ǵ�ǰ����	    
	u16 X; //LCD����/��ǰ���� 
	u16 Y;						   	    
	u8 Key_Sta;//�ʵ�״̬
//�ʼĴ���
//bit7:���ڱ�Ǵ��㱻�������,���ⲿ�����޸�
//bit1,0:��һ�ΰ��µİ���ģʽ(1:����;2,�㴥)
	u8 Key_Reg;//�ʼĴ���	  
//������У׼����
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;	   
extern Pen_Holder Pen_Point;
//�봥����оƬ��������	   
#define PEN  PBin(1)   //PF7  INT
#define DOUT PBin(0)   //PF8  MISO
#define TDIN PDout(13)  //PF9  MOSI
#define TCLK PDout(3) //PF10  SCLK
#define TCS  PDout(6) //PF11 CS 
//#define BUSY PDin(12)  //PF11 BUSY
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ָ�
#define CMD_RDY 0X90  //0B10010000���ò�ַ�ʽ��X����
#define CMD_RDX	0XD0  //0B11010000���ò�ַ�ʽ��Y����   											 
#define TEMP_RD	0XF0  //0B11110000���ò�ַ�ʽ��Y����        
			  
void Touch_Init(void);		 //��ʼ��
u8 Read_ADS(u16 *x,u16 *y);	 //��������˫�����ȡ
u8 Read_ADS2(u16 *x,u16 *y); //����ǿ�˲���˫���������ȡ
u16 ADS_Read_XY(u8 xy);		 //���˲��������ȡ(������)
u16 ADS_Read_AD(u8 CMD);	 //��ȡADת��ֵ					  
void ADS_Write_Byte(u8 num); //�����оƬд��һ������
void Draw_Touch_Point(u16 x,u16 y);//��һ�������׼��
void Draw_Big_Point(u16 x,u16 y);  //��һ�����
void Touch_Adjust(void);          //������У׼
#ifdef ADJ_SAVE_ENABLE
void Save_Adjdata(void);		  //����У׼����
u8 Get_Adjdata(void); 			  //��ȡУ׼����
#endif
void Pen_Int_Set(u8 en); 		  //PEN�ж�ʹ��/�ر�
u8 Convert_Pos(void);           //���ת������
void EXTI1_IRQRoutine(void);     //pen�жϺ��� 

u16 T_abs(u16 m,u16 n);      //������֮��ľ���ֵ
u8 AI_Read_TP(u8 MS_TIME,u8 interval);   //��ȡTP״̬	 
u8 Is_In_Area(u16 x,u16 y,u16 xlen,u16 ylen);//�жϰ����Ƿ������������.  	 
#endif

















