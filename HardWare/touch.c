 
#include "lcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
#include "usart.h"
#include "flash.h"
#include "sys.h"
#include "touch.h"	 
//Mini STM32������
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ��������
//����ԭ��@ALIENTEK
//2010/6/13
//V1.3	

Pen_Holder Pen_Point;//�����ʵ��
//SPIд����
//��7843д��1byte����   
void ADS_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0;//��������Ч	   	 
		TCLK=1;      
	} 			    
} 		 
//SPI������ 
//��7846/7843/XPT2046/UH7843/UH7846��ȡadcֵ	   
u16 ADS_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;//������ʱ�� 	 
	TCS=0; //ѡ��ADS7843	 
	ADS_Write_Byte(CMD);//����������
	Delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=1;//��1��ʱ�ӣ����BUSY   	    
	TCLK=0; 	 					 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK=0;//�½�����Ч  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   //ֻ�и�12λ��Ч.
	TCS=1;//�ͷ�ADS7843	 
	return(Num);   
}
//��ȡһ������ֵ
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
#define READ_TIMES 5 //��ȡ����
#define LOST_VAL 2	  //����ֵ
u16 ADS_Read_XY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//���˲��������ȡ
//��Сֵ��������100.
u8 Read_ADS(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}	
//2�ζ�ȡADS7846,������ȡ2����Ч��ADֵ,�������ε�ƫ��ܳ���
//50,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
#define ERR_RANGE 50 //��Χ 
u8 Read_ADS2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
	Delay_us(100);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
//��ȡһ������ֵ	
//������ȡһ��,֪��PEN�ɿ��ŷ���!					   
u8 Read_TP_Once(void)
{
	u8 t=0;	    
	Pen_Int_Set(0);//�ر��ж�
	Pen_Point.Key_Sta=Key_Up;	
	while(PEN==0&&t<=50)
	{
		t++;
		Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
		Delay_ms(50);
	}
	Pen_Int_Set(1);//�����ж�		 
	if(t>=50)return 0;//����2.5s ��Ϊ��Ч
	else return 1;	
}

//�õ�������֮��ľ���ֵ
u16 T_abs(u16 m,u16 n)
{
	if(m>n)return m-n;
	else return n-m;
}
//���ܶ�ȡ����������		   
//����ֵ0��û���κΰ�������
//����ֵ1������������,���������ƶ���
//����ֵ2����������,û���ƶ�
//PEN�ź�������ⰴ���Ƿ��ɿ�
//2010/6/19 �޸��˼���СBUG V0.0.2
#define PT_RANGE 3//���ΰ��µ�ķ�Χֵ
#define RANGE_LIMIT 15 //�ƶ���ǰ�����α䶯�ļ��ޣ��˳����
#define DELAYTIME 20  //��һ�ΰ��º���ʱ��ʱ�䣬�����˳��հ���ʹ�ö���
#define RETRYTIME 100  //����ʱ�䣬���尴�º���ʱһ��ʱ��������Ȼ���
			//��ת�����������������ʱ�������һ����Ч���������򱾴δ�����Ч
//#define PENTIME 50 //��ⴥ�ʰ��������ȶ�ʱ��
//����:MS_TIME ���ֻ����붨���ʱ��,һ������Ϊ30
//������Ϊ50ʱ,�Ƚ��ʺϻ������.����Ϊ30ʱ,�Ƚ��ʺ϶�����.
//��ֵ��Ӱ�쵽��������ٶ�.50��ʵҲ���Խ�����.
//���Ϊ255.
u8 AI_Read_TP(u8 MS_TIME,u8 interval)
{
	static u8 LSTA=0;
	static u8 delay=0;
	static u8 retry=0;	   
	u8 times;
	u16 tempX,tempY;		   
	switch(LSTA)
	{
		case 0:	  
		if(Pen_Point.Key_Sta==Key_Down)//�а���������
		{				 
			Pen_Int_Set(0);  //�ر��ж�
			if(delay++<DELAYTIME/interval) return 0;
			if(PEN) break;		 
			if(Convert_Pos())//ת��ΪLCD����	 
			{
				Pen_Point.X0=Pen_Point.X;//��¼��һ�ΰ��µİ�������λ��
				Pen_Point.Y0=Pen_Point.Y;
				times=0;
			}
			else
			{
				if(retry++<RETRYTIME/interval) return 0; 				
				else break;
			}				 
			do
			{
				tempX=Pen_Point.X;
				tempY=Pen_Point.Y;
				Convert_Pos();	   //�޸��������ϵ��ԭ��,ִ��һ����Ҫ��10ms���ҵ�ʱ��		 								 
				if((T_abs(Pen_Point.X,Pen_Point.X0)<=PT_RANGE)&&(T_abs(Pen_Point.Y,Pen_Point.Y0)<=PT_RANGE))
				times++;
				else if((T_abs(Pen_Point.X,Pen_Point.X0)<RANGE_LIMIT)&&(T_abs(Pen_Point.Y,Pen_Point.Y0)<RANGE_LIMIT))//�ƶ��������PT_RANGE
				{	
					Pen_Point.Key_Reg|=1;//��ǵ�һ�ΰ���Ϊ����ģʽ				 				 
					LSTA=1;
					return 1;//��������,�������ƶ�
				}
				else
				{Pen_Point.X = tempX; Pen_Point.Y = tempY;} 	    
				if(times>MS_TIME)//�涨ʱ����û���ƶ�����PT_RANGE��Χ,����������
				{
					Pen_Point.Key_Reg|=2;//��ǵ�һ�ΰ���Ϊ�㴥ģʽ						 			 
					LSTA=2;
					return 2;
				}
			}while(PEN==0);//����һֱ�ڰ���
			//�������������ɿ���,��Ϊ�̰�
			Pen_Point.Key_Reg|=2;  
			LSTA=2;
			return 2;
		}
		return 0;	  
		case 1://�����ڻ���	 
		if(PEN==0)
		{					    
			tempX=Pen_Point.X;
			tempY=Pen_Point.Y;
			Convert_Pos(); //��ȡ���겢ת��	
			if((T_abs(tempX,Pen_Point.X)>RANGE_LIMIT)||(T_abs(tempY,Pen_Point.Y)>RANGE_LIMIT))
			{Pen_Point.X = tempX; Pen_Point.Y = tempY;}
			Delay_us(MS_TIME<<2); //������!
			return LSTA;   //��Ȼ���ƶ���
		}
		break;		 
		case 2://����δ����	   
		if(PEN==0)
		{					    
			Convert_Pos(); //��ȡ���겢ת��ΪLCD����
			Delay_us(MS_TIME<<2);		   
			if((T_abs(Pen_Point.X,Pen_Point.X0)>PT_RANGE)||(T_abs(Pen_Point.Y,Pen_Point.Y0)>PT_RANGE))//�ƶ��������PT_RANGE
			{
				Pen_Point.Key_Reg|=1;
				LSTA=1;
				return 1;//��������,�������ƶ�
			}
			return 2;   //��Ȼû���ƶ�
		}		
		break;
	}
	Pen_Int_Set(1); //�����ж�
	Pen_Point.Key_Sta=Key_Up; 
	delay=0;
	retry=0; 						    
	LSTA=0;	   
	return 0;  	 
}
//�жϴ����ǲ�����ָ������֮��
//(x,y):��ʼ����
//xlen,ylen:��x,y�����ϵ�ƫ�Ƴ���
//����ֵ :1,�ڸ�������.0,���ڸ�������.
u8 Is_In_Area(u16 x,u16 y,u16 xlen,u16 ylen)
{
	if(Pen_Point.X<=(x+xlen)&&Pen_Point.X>=x&&Pen_Point.Y<=(y+ylen)&&Pen_Point.Y>=y)return 1;
	else return 0;
}  

//////////////////////////////////////////////////
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
void Draw_Touch_Point(u16 x,u16 y)
{
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_DrawCircle(x,y,6);//������Ȧ
}	  
//��һ�����
//2*2�ĵ�			   
void Draw_Big_Point(u16 x,u16 y)
{	    
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}
//////////////////////////////////////////////////

//ת�����
//���ݴ�������У׼����������ת����Ľ��,������X0,Y0��
u8 Convert_Pos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;
		return 1; 
	}
	return 0;	 
}	   
//�ж�,��⵽PEN�ŵ�һ���½���.
//��λPen_Point.Key_StaΪ����״̬
//�ж���0���ϵ��жϼ��
void EXTI1_IRQRoutine(void)
{ 		   			 
	  Pen_Point.Key_Sta=Key_Down;//��������  		  				 
	  EXTI_ClearFlag(EXTI_Line1);  //���LINE1�ϵ��жϱ�־λ 
} 
//PEN�ж�����	 
void Pen_Int_Set(u8 en)
{
	if(en)EXTI->IMR|=1<<1;   //����line1�ϵ��ж�	  	
	else EXTI->IMR&=~(1<<1); //�ر�line1�ϵ��ж�
	EXTI_ClearFlag(EXTI_Line1);
	NVIC_ClearIRQChannelPendingBit(EXTI1_IRQChannel);	   
}
	  
//////////////////////////////////////////////////////////////////////////
//�˲����漰��ʹ���ⲿEEPROM,���û���ⲿEEPROM,���δ˲��ּ���
#ifdef ADJ_SAVE_ENABLE
//������EEPROM����ĵ�ַ�����ַ,ռ��13���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
typedef struct
{
	//������У׼����
	float xfac;
	float yfac;
	short xoff;
	short yoff;
	u32 adj_mark;
}ADJ_PARA;
#define SAVE_ADDR_BASE 0x001000
//����У׼����										    
void Save_Adjdata(void)
{
	ADJ_PARA temp;			 
	//����У�����!		   							  
	temp.xfac=Pen_Point.xfac;	//����xУ������         
	temp.yfac=Pen_Point.yfac;	//����yУ������
	temp.xoff=Pen_Point.xoff;   //����xƫ����
	temp.yoff=Pen_Point.yoff;	//����yƫ����	
	temp.adj_mark=0x454A4441;	//ADJE ��ʾ��׼��������	     							 

	SPI_Flash_Write((u8 *)(&temp), SAVE_ADDR_BASE, 16);			 
}
//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
u8 Get_Adjdata(void)
{					  
	ADJ_PARA temp;
	SPI_Flash_Read((u8 *)(&temp), SAVE_ADDR_BASE, 16);   //��У׼���� 		 
	if(temp.adj_mark==0x454A4441)   //�������Ѿ�У׼����			   
	{    												 		   
		Pen_Point.xfac=temp.xfac;	//�õ�xУ׼����			          
		Pen_Point.yfac=temp.yfac;	//�õ�yУ׼����	    			   	  
		Pen_Point.xoff=temp.xoff;	//�õ�xƫ����			 	    				 	  
		Pen_Point.yoff=temp.yoff;	//�õ�yƫ����			 
		return 1;	 
	}
	return 0;
}
#endif		 
//������У׼����
//�õ��ĸ�У׼����
void Touch_Adjust(void)
{								 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	   
	cnt=0;				
	LCD_SetTextColor(Red);
	LCD_SetBackColor(White);
	LCD_Clear(White);//���� 
	Draw_Touch_Point(20,20);//����1 
	Pen_Point.Key_Sta=Key_Up;//���������ź� 
	Pen_Point.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//����������
		{
			if(Read_TP_Once())//�õ����ΰ���ֵ
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
				Delay_ms(100);
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(White);//���� 
					Draw_Touch_Point(300,20);//����2
					break;
				case 2:
					LCD_Clear(White);//���� 
					Draw_Touch_Point(20,450);//����3
					break;
				case 3:
					LCD_Clear(White);//���� 
					Draw_Touch_Point(300,450);//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
						Draw_Touch_Point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
						Draw_Touch_Point(20,20);
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x2-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y2-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�2,3�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x1-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y1-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�1,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
						Draw_Touch_Point(20,20);
						continue;
					}//��ȷ��
					//������
					Pen_Point.xfac=(float)280/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					Pen_Point.xoff=(320-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					Pen_Point.yfac=(float)430/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					Pen_Point.yoff=(480-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					LCD_SetTextColor(Blue);
					LCD_Clear(White);//����
					LCD_DisplayStringLine(Line9, 3, "Touch Screen Adjust OK!");//У�����
					Delay_ms(1000);
					LCD_Clear(White);//����   
					return;//У�����				 
			}
		}
	} 
}		    
//�ⲿ�жϳ�ʼ������
void Touch_Init(void)
{			    		   
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    //ʹ��GPIOFʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);
    //װ��GPIO_InitStructure����ԱĬ�ϳ�ʼֵ
    GPIO_StructInit(&GPIO_InitStructure);
	//PD.13(TDIN),PD.03(TCLK),PD.06(TCS) ͨ���������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	//PB.01(PEN),PB.00(DOUT)��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 	Read_ADS(&Pen_Point.X,&Pen_Point.Y);//��һ�ζ�ȡ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);                         
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
    //����EXTI_InitStructure����ԱΪĬ��ֵ
    EXTI_StructInit(&EXTI_InitStructure);

    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    //EXTI_Mode������EXTI_Mode_Interrupt��EXTI_Mode_Event
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //EXTI_Trigger������EXTI_Trigger_Rising��EXTI_Trigger_Falling
    //��EXTI_Trigger_Rising_Falling
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);			 
    //��ʼ��EXTI9_5�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
	//��ռ���ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	//�����ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	//�ж�ͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//��ʼ��EXTI9_5
	NVIC_Init(&NVIC_InitStructure);	 
#ifdef ADJ_SAVE_ENABLE	  
	if(Get_Adjdata())return;//�Ѿ�У׼
	else			   //δУ׼?
	{ 										    
	    Touch_Adjust();  //��ĻУ׼ 
		Save_Adjdata();	 
	}			
	Get_Adjdata();
#else
	LCD_Clear(White);//����
    Touch_Adjust();  //��ĻУ׼,���Զ�����			   
#endif
	printf("Pen_Point.xfac:%f\r\n",Pen_Point.xfac);
	printf("Pen_Point.yfac:%f\r\n",Pen_Point.yfac);
	printf("Pen_Point.xoff:%d\r\n",Pen_Point.xoff);
	printf("Pen_Point.yoff:%d\r\n",Pen_Point.yoff);
}

