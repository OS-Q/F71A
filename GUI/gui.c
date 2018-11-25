#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "fat.h"
#include "sysfile.h"
#include "jpegbmp.h"
#include "usart.h"
#include "sys.h"
//Mini STM32�����巶������28
//�ۺ� ʵ��
//����ԭ��@ALIENTEK

//ϵͳ���˵�
const u8 *Menu[12]=
{
"���ֲ���",			
"�������",
"����ͼ��",		  
" ����� ",
"ƴͼ��Ϸ",		 
"USB���",
"USB���� ",
"���ߴ���",	
"ʱ����ʾ",
"�ļ����",
"��������",
"ϵͳ�趨"	 
};
//////////////////////////GUI ��ť///////////////////////////////
void GUI_DrawLine(u16 x,u16 y,u16 xlen,u16 ylen,u16 color)
{
	u16 tempcolor=POINT_COLOR;
	POINT_COLOR=color;
	LCD_DrawLine(x,y,x+xlen,y+ylen);
	POINT_COLOR=tempcolor;	
}
//��ָ��λ����ʾָ����ɫ�İ�ť
//(x,y):��ť��ʼ����
//xlen,ylen:x,y����ĳ���
//str:��ť��Ҫ��ʾ���ַ�
//color:�ַ���ɫ
void GUI_Draw_Button(u16 x,u16 y,u16 xlen,u16 ylen,u8 *str,u16 color)
{	
	u16 tback_color,tpoint_color;
	//////////////////����ť����//////////////////////   
	LCD_Fill(x,y,x+xlen,y+ylen,LGRAY);//��ʵ��
	GUI_DrawLine(x,y,xlen,0,WHITE);   //����߿�
	GUI_DrawLine(x,y,0,ylen,WHITE);   //����߿�   	   
	GUI_DrawLine(x+1,y+1,xlen-1,0,0XB5B6);//���ڱ߿�
	GUI_DrawLine(x+1,y+1,0,ylen-1,0XB5B6);//���ڱ߿�
	GUI_DrawLine(x,y+ylen,xlen,0,0X8431); //����߿�
	GUI_DrawLine(x+xlen,y,0,ylen,0X8431); //����߿�   	   
	GUI_DrawLine(x+1,y+ylen-1,xlen-2,0,0XA535);//���ڱ߿�
	GUI_DrawLine(x+xlen-1,y,0,ylen-1,0XA535);  //���ڱ߿�
	//////////////////��ʾ��ť�������Ϣ///////////////  
	if(ylen<16)return;//������	 
	tback_color=BACK_COLOR;
	tpoint_color=POINT_COLOR;
	BACK_COLOR=LGRAY;
	POINT_COLOR=color; 
	Show_Str_Mid(x,y+(ylen-16)/2,str,16,xlen);//ֻ��16����
	BACK_COLOR=tback_color;	 //�ָ�֮ǰ����ɫ
	POINT_COLOR=tpoint_color; 
}
/////////////////////////����ͼ�μ���/////////////////////////////

//ѡ��˵�
void Select_Menu(u8 M_Type,u8 Cnt,u8 En)
{
	u16 tx,ty;
	u8 t;	 
	if(M_Type==0)//���˵�
	{	  
		tx=100*(Cnt%3)+30;//10,90,170			
		ty=110*(Cnt/3)+20;	
		//��ʾ��򲿷� 
		if(En)POINT_COLOR=BLUE;
		else POINT_COLOR=WHITE;
		for(t=0;t<4;t++)
		{
			LCD_DrawRect(tx-10+t,ty-10+t,tx+70-t,ty+70-t); 
		} 
		//��ʾ���壬����
		if(En)//ѡ�е�ǰ�Ĳ˵�
		{	  
			POINT_COLOR=WHITE;//��ɫ����
			BACK_COLOR=LBBLUE;  //��ɫ����      
		}else
		{
			POINT_COLOR=DARKBLUE;//����ɫ����
			BACK_COLOR=WHITE; //��ɫ����   
		}	 												  
		Show_Str(tx-2,ty+74,(u8*)Menu[Cnt],16,0);	
	}
}
//����������
void Load_Sys_ICO(void)
{		  
	u8 t;
	u16 ty,tx;	   
	FileInfoStruct temp; 
	POINT_COLOR=RED;//��ɫ����				   
    LCD_Clear(WHITE);//���� 
	temp.F_Type=T_BMP;//�ļ��������� 
	for(t=0;t<12;t++)
	{	   
		temp.F_StartCluster=sys_ico[t];								    
		tx=100*(t%3)+30;//25,115,205	
		ty=110*(t/3)+20;    
		AI_LoadPicFile(&temp,tx,ty,tx+59,ty+59);
		Select_Menu(0,t,0);//��������    		    
	}
}
//������Ϸ����
//size:3,3*3ͼƬ
//     4,4*4ͼƬ
//     5,5*5ͼƬ
//#define FRAME1 0XBC40 //��ɫ
void Load_Game_ICO(u8 size)
{
	u8 i;
	LCD_Clear(LGRAY);//����
	for(i=0;i<5;i++)//����߿�
	{		
		POINT_COLOR=FRAME1;
		LCD_DrawRect(i,i,319-i,319-i);
	}
	if(size==3)
	{
		for(i=0;i<5;i++)//��С�߿�
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRect(206+i,315+i,319-i,422-i);
		}									 
	    LCD_Fill(211,315,314,319,LGRAY);//�Ѷ����ȥ��	
		POINT_COLOR=RED;
		LCD_DrawRect(211,314,314,417);//���յķ������Ȳ���ͼƬ
		LCD_DrawRect(5,5,314,314); //�����ĺ��

		LCD_DrawLine(108,6,108,313);  //���ڲ���� ����
		LCD_DrawLine(211,6,211,313);//���ڲ����
		LCD_DrawLine(6,108,313,108);  //���ڲ���� ����
		LCD_DrawLine(6,211,313,211);//���ڲ����
	}else if(size==4)
	{
		for(i=0;i<4;i++)//��С�߿�
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRect(234+i,316+i,319-i,398-i);
		}									 
	    LCD_Fill(238,316,314,319,LGRAY);//�Ѷ����ȥ��	
		POINT_COLOR=RED;
		LCD_DrawRect(238,316,316,394);//���յķ��򣬵�ʮ����
		LCD_DrawRect(4,4,316,316);    //�����ĺ��

		for(i=1;i<size;i++)
		LCD_DrawLine(5,4+78*i,315,4+78*i);  //���ڲ���������
		for(i=1;i<size;i++)
		LCD_DrawLine(4+78*i,5,4+78*i,315);  //���ڲ���������	   										  
	}else if(size==5)
	{
		for(i=0;i<5;i++)//��С�߿�
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRect(247+i,315+i,319-i,382-i);
		}										 
	    LCD_Fill(252,315,314,319,LGRAY);//�Ѷ����ȥ��	
		POINT_COLOR=RED;
		LCD_DrawRect(252,314,314,377);//���յķ��򣬵�ʮ����
		LCD_DrawRect(4,4,314,314);    //�����ĺ��

		for(i=1;i<size;i++)
		LCD_DrawLine(5,4+62*i,313,4+62*i);  //���ڲ���������
		for(i=1;i<size;i++)
		LCD_DrawLine(4+62*i,5,4+62*i,313);  //���ڲ���������
	}	  		  								    	  		  
	LCD_DrawRect(2,340,105,443);//����Ԥ��ͼƬ
	
	BACK_COLOR=WHITE;
	POINT_COLOR=BLUE;//������ɫ
	Show_Str(110,350,"�ȼ�:",16,0x01);
	POINT_COLOR=RED;//������ɫ
	if(size==3)Show_Str(160,350,"��",16,0x01);
	else if(size==4)Show_Str(160,350,"һ��",16,0x01);
	else Show_Str(160,350,"����",16,0x01);
	POINT_COLOR=BLUE;//������ɫ
	Show_Str(110,370,"����:",16,0x01);
	Show_Str(110,390,"��ʱ:",16,0x01);   
	GUI_Draw_Button(130,420,40,24,"�˳�",BLACK);		  					     
}

void Load_Draw_Dialog(void)
{
	if(PEN == 0)
	{
		LCD_SetBackColor(Blue1);
		LCD_SetTextColor(Blue);//��������Ϊ��ɫ
		LCD_DisplayStringLine(Line0, 37, "RST");
		Delay_ms(200);
	}
	LCD_Clear(White);//���� 
	LCD_SetBackColor(White);  
	LCD_DisplayStringLine(Line0, 37, "RST");//��ʾ��������
	LCD_DisplayStringLine(Line0, 0, "EXIT");
  	LCD_SetTextColor(Red);//���û�����ɫ 
}
 		 	 
//�����������
//���ý��� 
//str:"ѡ��/ȷ��"
void Load_Gui_Bro(u8 * str)
{			 	   					  			   
    LCD_Clear(WHITE);//����
	//���� 						   
 	LCD_Fill(0,0,319,9,LIGHTBLUE);  //���ײ���ɫ(ǳ��ɫ)	
 	LCD_Fill(0,10,319,20,GRAYBLUE); //���ײ���ɫ(����ɫ)
	//�в�
	POINT_COLOR=DARKBLUE;//����ɫ  
	LCD_DrawLine(0,21,319,21);
	LCD_Fill(0,22,319,41,LIGHTBLUE);//����в�����ɫ(ǳ����ɫ)
	LCD_DrawLine(0,41,319,41);	    
	//�ײ�
	LCD_DrawLine(0,458,319,458);
 	LCD_Fill(0,459,319,468,LIGHTBLUE);//���ײ���ɫ(ǳ��ɫ)	
 	LCD_Fill(0,469,319,479,GRAYBLUE);//���ײ���ɫ(����ɫ)

	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"��ѡ��:",16,0x01); //����ģʽ,���Զ�����
	POINT_COLOR=WHITE;
	Show_Str(5,460,str,16,0x01);   //����ģʽ,���Զ�����
	Show_Str(283,460,"����",16,0x01); //����ģʽ,���Զ�����  
}
//������ʾ����
//��PS2/���ػ�ͼʱ�õ�
//���ý��� 
//str:"ѡ��/ȷ��"
void Load_Gui_Show(u8 *lbtstr,u8 *mbtstr,u8 *rbtstr)
{			 	   					  			   
    LCD_Clear(WHITE);  //����  
 	POINT_COLOR=DARKBLUE;//����ɫ	 
	LCD_DrawLine(0,457,319,457);
 	LCD_Fill(0,458,319,467,LIGHTBLUE);//���ײ���ɫ(ǳ��ɫ)	
 	LCD_Fill(0,468,319,479,GRAYBLUE); //���ײ���ɫ(����ɫ)	  	   
	POINT_COLOR=WHITE;//��ɫ����
	Show_Str(5,460,lbtstr,16,0x01);    //����ģʽ,���Զ�����
	Show_Str(144,460,mbtstr,16,0x01);  //����ģʽ,���Զ�����		  
	Show_Str(283,460,rbtstr,16,0x01);  //����ģʽ,���Զ�����  
}

////////////////////////////////�봥����������صĺ���////////////////////////////
//�Ѵ������õ�������ת��Ϊ����(��ťֵ)
//M_Type:��ťģʽ;
//0,ϵͳ�˵�	
//1,�ļ����ģʽ 	
//2,ʱ��ģʽ��,�����Ļ�ȡ,����֧��! 
//3,���ֲ���ģʽ��
//4,����ѡ��ģʽ��
//5,������ģʽ��
//6,�����жϳ��򰴼�

//7,TXT���/JPEG��� ʱ��������			  
//����ֵ:��Ӧ�İ���ֵ(1~n)
//����0,���ʾ�˴ΰ�����Ч  
u8 Touch_To_Num(u8 M_Type)
{
	u16 xtemp;
	u16 t,ytemp;
	switch(M_Type)
	{
		case 0://ϵͳ�˵���										  
			for(t=0;t<12;t++)
			{   
				xtemp=100*(t%3)+30;
				ytemp=110*(t/3)+20;
				if(Is_In_Area(xtemp,ytemp,60,90))return t+1;//���ذ���ֵ 
			}
			return 0;			  
		case 1://1,�ļ����ģʽ	   	  
			for(t=0;t<13;t++)
			{	  
				if(Is_In_Area(0,42+t*32,160,32))return t+1;
			}
			if(Is_In_Area(0,450,40,30))return 14;//ѡ��ť������
			if(Is_In_Area(280,450,40,30))return 15;//���ذ�ť������
			return 0;//������Ч	    
//			if(xtemp)return xtemp&0xf0;//����������!! 
			//break; 
		case 2://4,����ѡ��ģʽ�� 
			for(t=0;t<13;t++)
			{	  
				if(Is_In_Area(0,42+t*32,320,32))return t+1;
			}
			if(Is_In_Area(0,450,40,30))return 14;//ѡ��ť������
			if(Is_In_Area(280,450,40,30))return 15;//���ذ�ť������
			return 0;//������Ч	 
			//break;

		case 3:
			if(Is_In_Area(280,270,30,15))return 1;//ѭ��ģʽ����	    
			else if(Is_In_Area(200,270,30,15))return 2; //EQ
			else if(Is_In_Area(240,270,30,15))return 3;   //LRC
			else if(Is_In_Area(VOLBAR_X,VOLBAR_Y-12,78,13))return 4;//����
			else if(Is_In_Area(290,5,20,20))return 5;//�ر�	 
				
			else if(Is_In_Area(20,240,30,30))return 6;//��һȡ
			else if(Is_In_Area(75,230,50,50))return 7;//����
			else if(Is_In_Area(150,240,30,30))return 8;//��һ��

			else if(Is_In_Area(20,195,280,10))return 9;//����������,y����������һ�㷶Χ

			else
			return 0;//������Ч
		case 4:						    
			break;
		case 5://5,������ģʽ�� 
			break; 
		case 6://6,�����жϳ��򰴼�	  
			break;
		case 7://JPEG/BMP/TXT�������ģʽ  
			if(Is_In_Area(0,0,319,120))return KEY_PREV;        //����
			else if(Is_In_Area(0,120,319,240))return KEY_FUNC; //����		    
			else if(Is_In_Area(0,360,319,120))return KEY_NEXT; //���·� 	   
			return 0;//������Ч	    			
			//break; 	  	    		 
	}
	return 0;
}
//�����������
u8 KEY_Move_Check(void)
{
#define MOVE_PIXES 50 //��������						    
	if(Pen_Point.X>Pen_Point.X0)//����
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_RIGHT;
	}else  //����
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_LEFT;
	}
	if(Pen_Point.Y>Pen_Point.Y0)//����
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_DOWN;
	}else			   //����
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_UP;
	}	 
	return 0;							 
}
//////////////////////////////���ļ������صĺ���//////////////////////////////
//����ѡ����Ⱥ���	    
//list  ��������Ŀָ��
//title ������
//func  ��ѡ��/ȷ��/������
//len   ��������Ŀ�ĸ���(1~8)
//sel   ����ʼѡ�е���Ŀ(0~7)
//����ֵ��0~7,ѡ�е���Ŀ��8��û��ѡ���κ���Ŀ��
u8 Load_Fun_Sel(const u8 * *list,const u8 *title,u8 *func,u8 len,u8 sel)
{	
	u8 key;
	u16 t;
	Load_Gui_Bro(func);	   
	//��ʾ����
	POINT_COLOR=WHITE;
	Show_Str_Mid(0,2,(u8*)title,16,320); //����ģʽ,���Զ�����  
	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"��ѡ��:",16,0x01);    //����ģʽ,���Զ�����	 
	//������Ŀ��Ϣ
	for(t=0;t<len;t++)
	{	 											    
		if(t==sel)
		{
			LCD_Fill(0,45+sel*32,319,37+(sel+1)*32,LBBLUE );//��ѡ�е���Ŀ�������ɫ 
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;	  
		Show_Str(0,50+t*32,(u8*)list[t],16,0x01); //����ģʽ,���Զ�����       
	} 
 	key=0;
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���	    
	while(1)//�ȴ�����
	{	 
		key=AI_Read_TP(30,1);
		if(key)Pen_Point.Key_Reg|=0X80;//��ǰ����Ѿ������� 	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//�����ɿ���
		{
			Pen_Point.Key_Reg=0;			    
			key=Touch_To_Num(2);
			if(key)//�а�������
			{  									    
				switch(key)
				{
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
					case 14://ȷ�ϼ�����
					key-=1;
					if(sel!=key&&key<len)//ѡ��һ��
					{
						POINT_COLOR=DARKBLUE;//������ɫѡ��		 
						LCD_Fill(0,45+sel*32,319,37+(sel+1)*32,WHITE);//����ϴ�ѡ���
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//������ʾ�ϴε�����
			        	sel=key;//ѡ�����ѡ���
						LCD_Fill(0,45+sel*32,319,37+(sel+1)*32,LBBLUE);//����ɫ
						POINT_COLOR=WHITE;//������ɫѡ��		 
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//��ʾ���� 
					}else if(sel==key||key==13)//ѡ��ť,����˫��
					{	 	
						return sel;//����ѡ�����Ŀ��
					}
						break;  		  	  
					case 15:return 13;//ѡ����"����"��ť,����һ��������ֵ  
				}  
				key=0;//ȡ������ֵ
			}											    
		}	  
		Delay_ms(1);
		t++;
		if(t%500==0) SignalLED=!SignalLED;	
	}  
}
//��ʾ�ļ�
//fcnt:Ҫ��ʾ�ļ��Ķ���
//folder:�ļ��и���
//sel :ѡ�еڼ���1~8 
//����ֵ:Ҫ�򿪵��ļ����1~8(�����ļ���,��FileInfo�����)	    
//����9,����ֱ���˻���һ��Ŀ¼  		    
//��ʾ�ļ�
//check ok 09/04/19
u8 Show_File(u8 fcnt,u8 folder,u8 sel)
{
	u16 t;	
	u8 key; 					   
	FileInfoStruct temp;
	temp.F_Type=T_BMP;//�ļ���������

	sel-=1;										 
	LCD_Fill(0,42,319,457,WHITE);//����ɫ,����ϴε���ʾ
	for(t=0;t<fcnt;t++)
	{
		if(F_Info[t].F_Type==T_FILE)temp.F_StartCluster=file_ico[0];//�ļ���ͼ��
		else 
		{
			if(F_Info[t].F_Type&(T_JPG|T_JPEG|T_BMP))temp.F_StartCluster=file_ico[2];//ͼƬ��ͼ��
			else if(F_Info[t].F_Type&(T_MP3|T_OGG|T_WMA|T_WAV|T_FLAC|T_MID))temp.F_StartCluster=file_ico[1];//������ͼ��
			else if(F_Info[t].F_Type&(T_LRC|T_TXT|T_C|T_H))temp.F_StartCluster=file_ico[3];//������ͼ��
			else temp.F_StartCluster=file_ico[4];//δ֪�ļ�ͼ��
		}				   
		AI_LoadPicFile(&temp,0,42+t*32,31,42+t*32+31);//����ͼ��  
		if(t==sel)
		{
			LCD_Fill(32,45+sel*32,319,37+(sel+1)*32,LBBLUE);//�����ɫ BROWN	  
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;
		Show_Str(32,50+t*32,F_Info[t].F_Name,16,0x03); //����ģʽ,���Զ����� 
		//printf("N[%d]%s\n",t,F_Info[t].F_Name);      
	}
	key=0;
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
	while(1)//�ȴ���������
	{
		key=AI_Read_TP(50,1);//�ʺϻ������	    
		if(key)
		{
			if((Pen_Point.Key_Reg&0x01)==1)//��һ���ǻ�������
			{
				t=KEY_Move_Check();//��黬������
				if(t)
				{
					key=t;  
					goto KEY_D;
				}
			}
			Pen_Point.Key_Reg|=0X80;  //��ǰ����Ѿ�����
		}	   
		else if(Pen_Point.Key_Reg&0X80)//�����ɿ���
		{
			key=Touch_To_Num(1);
KEY_D:	    
			Pen_Point.Key_Reg=0;		  	
			if(key)//�а�������
			{
				switch(key)
				{
					case 1:
					case 2:
			 		case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
						key-=1;
						if(sel!=key&&key<fcnt)//ѡ��һ��
						{
							POINT_COLOR=DARKBLUE;//������ɫѡ��		 
							LCD_Fill(32,45+sel*32,319,37+(sel+1)*32,WHITE);//����ϴ�ѡ���
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//������ʾ�ϴε�����
				        	sel=key;//ѡ�����ѡ���
							LCD_Fill(32,45+sel*32,319,37+(sel+1)*32,LBBLUE);//����ϴ�ѡ���
							POINT_COLOR=WHITE;//������ɫѡ��		 
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//��ʾ���� 
						}else if((sel==key)&&(BrowseAll||sel<folder))
						 return sel+1;//�ٴ�ѡ��,˫����.
						break;
					case 14: //ѡ����"ѡ��"��ť
						return sel+1; 	  
					case 15://ѡ����"����"��ť 
						return 14;
					case MOVE_UP:  //��ҳ���� 
					case MOVE_LEFT: 
				    case MOVE_DOWN:  
					case MOVE_RIGHT: 
						return key;
				}
				key=0;//�Ѱ���ֵȡ����				  
				Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
			}  
		}
		else{}
		Delay_ms(1);
		t++;
		if(t%500==0) SignalLED=!SignalLED;	
	}
}
u8 BrowseAll = 1; 
//����ļ���������ļ�
//file_type:Ҫ������ļ�����
//����Ҫ�򿪵�Ŀ���ļ���� 1~n(����Ŀ���ļ������)
//�������ļ���!!!
//�������0������˳����
u16 File_Browse(u32 file_type)
{			 
	FileInfoStruct *FileInfo;
	u16 folder_cnt;//�ļ��еĸ���
	u16 des_cnt;   //Ŀ���ļ��ĸ���		    
	u8 fcnt;	   //��ʾ���ļ���Ŀ
	u16 temp; 					     									  

	u16 total_file;//���ļ���Ŀ
	u16 pos_cnt;//��ǰ�ļ��� 1~folder_cnt+des_cnt
	u8 key;	  			  
							   			  
Reset:				    
	POINT_COLOR=BLUE;  	   		   //��ɫ��ͷʹ��
	folder_cnt=0;des_cnt=0;fcnt=0;key=0;pos_cnt=0;	 	
	Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&folder_cnt);//�õ���ǰĿ¼���ļ��еĸ���	    
	Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&des_cnt);//�õ���ǰĿ¼��Ŀ���ļ��ĸ���	 
									   
	total_file=des_cnt+folder_cnt;//�ܵ��ļ���Ŀ=�ļ�����Ŀ+Ŀ���ļ���Ŀ
	while(1)
	{
		if(pos_cnt<total_file)//�ļ����,δ�����ļ�ĩβ
		{	   
			FileInfo=&F_Info[fcnt];
			if(pos_cnt<folder_cnt)//���ļ���
			{
			    temp=pos_cnt+1;			    
				Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&temp);
			}else				   //��Ŀ���ļ�
			{
				temp=pos_cnt-folder_cnt+1; 
				Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&temp);
			}
			fcnt++;   //�Ѵ���ļ���Ŀ����
			pos_cnt++;//�ļ�λ�ñ�־����
		}	  
		if(fcnt==13||pos_cnt==total_file)//�����˸���,�����ļ����Ѿ�������
		{  
			key=Show_File(fcnt,folder_cnt,1);//��ʾ�˴εõ����ļ�
			switch(key)
			{
				case 1://Ҫ��Ŀ���ļ�
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					key-=1;
					if(key<fcnt)//��������Ч��Χ��
					{
						if(F_Info[key].F_Type==T_FILE)//Ҫ�򿪵��Ǹ��ļ���
						{
							Cur_Dir_Cluster=F_Info[key].F_StartCluster;//������ļ���		  
							goto Reset;
						}else    //Ҫ�򿪵���Ŀ���ļ�
						{	
							if(BrowseAll)		   
							return pos_cnt+key-folder_cnt-fcnt+1;//����Ŀ���ļ����
						}
					}
					break;
				case 14:	//ѡ���˷��ذ�ť.�˳�����һ��Ŀ¼
					//����Ŀ¼�������,���·��ذ�ť,���˳����ģʽ	
					if((FAT32_Enable&&Cur_Dir_Cluster==FirstDirClust)||Cur_Dir_Cluster==0)return 0;//�˳����ģʽ	  					
					Cur_Dir_Cluster=Fat_Dir_Cluster;//�˵���Ŀ¼  			    	 
					goto Reset;	   	 
				case MOVE_UP:  //���Ϸ�ҳ
				case MOVE_LEFT:
					if(pos_cnt>13)pos_cnt-=(13+fcnt);//�ص���һҳ
					else pos_cnt=(total_file%13)?(total_file/13*13):(total_file-13);//��ͷ��ʼ��
					break;
				case MOVE_DOWN://���·�ҳ
		    	case MOVE_RIGHT:
					if(pos_cnt==total_file)pos_cnt=0;//������
					break;

			}	 				    
			fcnt=0;//�����ʾ�ļ�������
		}
	}		
}

//����MP3���Ž���
//sysfun������0~9bit������ǲ�ͬ״̬
void Load_Gui_Mp3(u16 funmark)
{	 
	u16 t;
	u16 temp;
	u16 GraPer;
	u8 n,m,k;
	if(funmark)	goto UDM;	//�жϼ��ر�ǣ��������Ѽ��ع�����������������
	for(t=0;t<30;t++)  //��������������
	{
		if(t<10) GraPer=t*25;
		else GraPer=t*25+(t-15)*25;	//����ɫ�ٷֱȼ���
		temp=GradualColor(0x0001,0x5AF0,GraPer);
		LCD_Fill(0,t,319,t,temp);   //��䶥����ɫ
	}
	POINT_COLOR=0xB577;
	Show_Str_Mid(0,7,"���ֲ�����",16,320);
	TFT_CloseButton(290,5,20,0x632E,0x2105); //���ƹرհ�ť
	POINT_COLOR=0x20A9;
	LCD_DrawRect(10,30,309,189); //���ƾ��ο򣬴ﵽ����Ч��
UDM:
	for(t=31;t<44;t++)	//���������ƣ���ʾ������������
	{					//���������ֲ���ʱ��������Ҫ�ػ�
		GraPer=(t-31)*85;
		temp=GradualColor(0x52B0,0x420C,GraPer);
		LCD_Fill(11,t,308,t,temp);   //��䶥����ɫ
	}
	LCD_Fill(11,44,308,54,0x0863);	 //�������²���ɫ��
	LCD_Fill(11,57,308,188,0x28CA);	//���FFT��ʾ����
	if(funmark) return;	//�����Ѽ��ع����򷵻�

	LCD_Fill(11,55,308,55,0x424D); //��������FFT��ʾ��ϴ�����
	temp=GradualColor(0x28CA,0xFFFF,100);
	LCD_Fill(11,56,308,56,temp);	//ʵ��������͹�������Ч��

	LCD_Fill(0,30,9,189,0x5AF0);
	LCD_Fill(310,30,319,189,0x4A6E);//���Ʋ����������

	for(t=190;t<300;t++)  //�������²�������ɫ
	{
		GraPer=(t-190)*9;
		temp=GradualColor(0x5AF0,0x10C4,GraPer);
		LCD_Fill(0,t,319,t,temp);   
	}
	for(t=206;t<218;t++) //ȡ�������·�����ʱ����������ַ���ʾ����
	{					 //������ɫ������д�ò����ַ�ʱʹ��
		BGC_Buffer[t-206][0]=LCD_ReadPoint(160,t);
	}
	temp=LCD_ReadPoint(30,190);//FFT��ʾ�����±�Ե��ɫ����
	temp=GradualColor(temp,0xFFFF,150);
	LCD_Fill(11,190,308,190,temp);

	temp=LCD_ReadPoint(30,195);	//�������ϱ�Ե��ɫ����
	temp=GradualColor(temp,0x0000,150);
	LCD_Fill(20,195,299,195,temp);
	temp=LCD_ReadPoint(30,204);//�������±�Ե��ɫ����
	temp=GradualColor(temp,0xFFFF,150);
	LCD_Fill(20,204,299,204,temp);
	for(t=195;t<205;t++)	  //��������������Ե����Ч��
	{
		temp=LCD_ReadPoint(19,t);
		POINT_COLOR=GradualColor(temp,0xFFFF,150);
		LCD_DrawPoint(19,t);   //���Ե��ɫ����
		POINT_COLOR=GradualColor(temp,0x0000,150);
		LCD_DrawPoint(300,t);  //�ұ�Ե��ɫ����
	} 

	for(t=0;t<110;t++)	  //Ƶ��ɫ���������
	{
		GraPer=t*9;
		temp=GradualColor(0x7DDE,0x5B16,GraPer);	
		FFT_Color[t]=temp;
	}	  
	TFT_DrawCirque(100,255,29,26,0x5A8E);//������ť���Ե��ɫ�ν�
	TFT_DrawCirque(35,255,19,16,0x5A8E);
	TFT_DrawCirque(165,255,19,16,0x5A8E);//ǳɫ������

	TFT_DrawCirque(100,255,31,30,0x0002);
	TFT_DrawCirque(35,255,21,20,0x0002);
	TFT_DrawCirque(165,255,21,20,0x0002); //��ɫ������

	TFT_DrawCButton(100,255,25,0xACF9,0x2910,2);//���Ʋ��Ű�ť
	TFT_DrawCButton(35,255,15,0xACF9,0x2910,3);//������һ����ť
	TFT_DrawCButton(165,255,15,0xACF9,0x2910,4);//������һ����ť

	LCD_WriteCom(0x0036);LCD_WriteData(0x002a);	//LCDɨ�跽ʽ�趨,X\Y���꽻��
	LCD_ReadRect(VOLBAR_Y-12,VOLBAR_X,VOLBAR_Y,VOLBAR_X+77,BGV_Color);//��ȡ������ʾ����ı�����ɫ
																	 //���ػ�ʱҪ�õ�
	POINT_COLOR = 0x39EB;
	LCD_SetDisplayWindow(VOLBAR_Y-12,VOLBAR_X,VOLBAR_Y,VOLBAR_X+77);//�趨����	
	for(n=0;n<26;n++)	 //�������������
	{
		for(m=0;m<3;m++)
		for(k=0;k<13;k++)
		{
			if(12-k>n/2)
			LCD->LCD_RAM = BGV_Color[n*39+m*13+k];
			else if(12-k==n/2)
			LCD->LCD_RAM = GradualColor(BGV_Color[n*39+m*13+k],0x0000,200);//������������ϱ�Ե
			else
			LCD->LCD_RAM = POINT_COLOR;
		}
	}

	LCD_ReadRect(VOLBAR_Y-12,VOLBAR_X,VOLBAR_Y,VOLBAR_X+77,BGV_Color);
	LCD_WriteCom(0x0036);LCD_WriteData(0x000a); //��ȡ������ɫ����
	
	temp=LCD_ReadPoint(VOLBAR_X,VOLBAR_Y+1);
	POINT_COLOR=GradualColor(temp,0xFFFF,150); //������������±�Ե
	LCD_Fill(VOLBAR_X,VOLBAR_Y+1,VOLBAR_X+77,VOLBAR_Y+1,POINT_COLOR);//ʵ������Ч��
	TFT_DrawSpeak(VOLBAR_X-30,VOLBAR_Y-5);

	TFT_FucButton(200,270,30,0x632E,0x2105,"EQ");//����EQ��ť
	TFT_FucButton(240,270,30,0x632E,0x2105,"LRC");//����LRC��ť
	TFT_FucButton(280,270,30,0x632E,0x2105,"A");//����ѭ��ģʽ��ť
											   //Ĭ��ȫ��ѭ��
	LCD_Fill(0,300,319,479,BLACK);	//�����ʾ����
}
/*
//������Ч�Ľ���
//kind:0:VS1003 
//kind:1:PT2314
void Load_Gui_Set_EQ(u8 kind)
{			  
	LCD_Clear(WHITE);//���� 	   
	//������
	LCD_Fill(0,0,239,20,MP3COLOR1);   //��䶥����ɫ
	POINT_COLOR=GRED;
	BACK_COLOR=MP3COLOR1;
	Show_Str_Mid(0,3,"��Ч����",16,240);   

	LCD_Fill(0,21,239,41,Bro_MID);  //����в�����ɫ
	LCD_Fill(0,298,239,319,Bro_BTM);//���ײ���ɫ	    

	LCD_Fill(0,74,239,106,LGRAY); //��䱳ɫ
	LCD_Fill(0,138,239,170,LGRAY);//��䱳ɫ 
	
	POINT_COLOR=BLUE;
	if(kind) //����PT2314
	{
		LCD_Fill(0,202,239,234,LGRAY);//��䱳ɫ    
		Show_Str(0,50,"����:",16,0x01);   //����ģʽ,���Զ�����
		Show_Str(0,82,"����:",16,0x01);   //����ģʽ,���Զ�����
		Show_Str(0,114,"����:",16,0x01);   //����ģʽ,���Զ�����
	
		Show_Str(0,146,"������:",16,0x01);   //����ģʽ,���Զ�����
		Show_Str(0,178,"������:",16,0x01);   //����ģʽ,���Զ�����
		Show_Str(0,210,"������:",16,0x01);   //����ģʽ,���Զ�����	 
	}else//����VS1003
	{
		Show_Str(0,50,"����:",16,0x01);   //����ģʽ,���Զ�����
		Show_Str(0,82,"��Ƶ:",16,0x01);   //����ģʽ,���Զ�����
		Show_Str(0,114,"����:",16,0x01);   //����ģʽ,���Զ�����
	
		Show_Str(0,146,"��Ƶ:",16,0x01);   //����ģʽ,���Զ�����
		Show_Str(0,178,"����:",16,0x01);   //����ģʽ,���Զ�����
	}   				    
	//���� 
	POINT_COLOR=BROWN;
	BACK_COLOR=WHITE;
	TFT_ShowButton(100,46,1);//<
	TFT_ShowButton(172,46,0);//>
	//����
	TFT_ShowButton(100,110,1);//<
	TFT_ShowButton(172,110,0);//>
	//������
	TFT_ShowButton(100,174,1);//<
	TFT_ShowButton(172,174,0);//>
	//����
	BACK_COLOR=LGRAY;
	TFT_ShowButton(100,78,1);//<
	TFT_ShowButton(172,78,0);//>
	//������
	TFT_ShowButton(100,142,1);//<
	TFT_ShowButton(172,142,0);//>
	//������
	if(kind)
	{
		TFT_ShowButton(100,208,1);//<
		TFT_ShowButton(172,208,0);//>
		POINT_COLOR=RED;
		Show_Str(5,300,"ѡ��",16,0x01);   //����ģʽ,���Զ�����
	}else
	{
		POINT_COLOR=RED;
		Show_Str(5,300,"ȷ��",16,0x01);   //����ģʽ,���Զ�����
	}
	//����ѡ����ذ�ť						   		    

	Show_Str(203,300,"����",16,0x01); //����ģʽ,���Զ�����  
} 			
*/
//////////////////////////��LCD��ʾ��صĺ���//////////////////////////////////
//������
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawDLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		Draw_Big_Point(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 	 
//����ɫ�б�
//����ɫ������СΪ51*37
//�ֳ�5*7����ɫ��,�ܹ�35����ɫ
const u16 Color_Tab[3][7]=
{
{0XF800,0X7E00,0X001F,0X0000,0XFFFF,0X8000,0X0400},
{0XFFE0,0X07FF,0XF81F,0X2B12,0X8400,0XFFF0,0XFC10},
{0X0010,0X841F,0X041F,0XFC10,0X87F0,0XF810,0XFC00},
};
//����ɫ�б�
//����ɫ������СΪ65*29
//�ֳ�3*7����ɫ��,�ܹ�21����ɫ
//#define FRAME1 0XEF5B //����ɫ 	 
//#define FRAME2 0XDED7 //����ɫ	 
void Draw_Color_Table(u16 x,u16 y)
{	
	u8 t,i;	  
	POINT_COLOR=FRAME2;
	for(i=0;i<4;i++) 
	{	 
		LCD_DrawLine(x,y+i*9,x+63,y+i*9);
	}
	for(i=0;i<4;i++) 
	{					    
		LCD_DrawLine(x,y+i*9+1,x+63,y+i*9+1);
	}		    
	for(i=0;i<8;i++) 
	{	 
		LCD_DrawLine(x+9*i,y,x+9*i,y+28);
	}
	for(i=0;i<8;i++)//��С�߿�
	{					    
		LCD_DrawLine(x+9*i+1,y,x+9*i+1,y+28);
	}
	for(t=0;t<3;t++)
	{
		for(i=0;i<7;i++)
		{
			LCD_Fill(x+2+i*9,y+2+t*9,x+2+i*9+6,y+2+t*9+6,Color_Tab[t][i]);//�Ѷ����ȥ��
		}
	}								    	  
}










