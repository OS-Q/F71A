#include "ptgame.h"
#include "gui.h"
#include "sys.h"
#include "sysfile.h"
#include "lcd.h"
#include "delay.h"
#include "fat.h"
#include "jpegbmp.h"
#include "touch.h"
#include "text.h"
#include "stdlib.h"


//ƴͼ��Ϸ����
//֧�� 3*3 4*4 5*5�����ȼ���Ҫ�������Ҳ�ǱȽ����׵�
//����ֻҪ�޸�ͼƬ���Cur_Table�Ĵ�С��PT_Size�Ĵ�С�Ϳ�����
//����Ҫע�ⲻҪ�����������͵ķ�Χ						    
//����ԭ��@SCUT
//2009/05/30 
//V1.2												  

//�洢��sram����� ͼƬ��ʼ��ַ����SD���ϣ� �б�
//u32 *Pic_Addr=(u32*)iclip;//ע�� 0����ʾԤ��ͼƬ�ĵ�ַ 1~n����ʾ1~n��ͼƬ�ĵ�ַ
u8 Cur_Table[26];//ͼƬ��Ŵ洢�б�	5*5ʱ��СΪ25+1=26

u8 Xscal;        //������
//���ĸ�������Ҫ���浽EEPROM����
u8 PT_Size=3;    //ƴͼ��Ϸ���Ѷȼ��� 3��4����ֵ

u8  PT_New=0; //�Ƿ��Ѿ����棿1��û�����棬������¡�0���Ѿ����棬����δ��ɣ����Բ����¡�
u16 PT_Step=0;//ƴͼ���ò���
u16 PT_Time=0;//ƴͼ����ʱ��	
								   
const u8 *PTGmenu_F1[1]={"ƴͼ��Ϸ"};//��1����Ŀ¼Ŀ¼
const u8 *PTGmenu_S1[4]={"����","����Ϸ","�������","��Ϸ����"};//��1����Ŀ¼
const u8 *PTGmenu_S2[3]={"��","һ��","����"}; //��2����Ŀ¼ ����	 

#define PT_BASIC 1 //������Ϣ
#define PT_TABLE 2 //ͼ����Ϣ	 

//���浱ǰ��Ϸ��Ϣ
//��ַ���䣺412~443
void Save_PTG_Info(u8 type)
{			 
	u8 i;
	if(type==PT_BASIC)//���������Ϣ
	{								 
		BKP_WriteBackupRegister(BKP_DR7,PT_Size);//����ͼƬ��С
		BKP_WriteBackupRegister(BKP_DR8,PT_New); //�����Ƿ��Ѿ�����
		//���浱ǰ���ò���
		BKP_WriteBackupRegister(BKP_DR9,PT_Step);	
		//���浱ǰ����ʱ��
		BKP_WriteBackupRegister(BKP_DR10,PT_Time);
	}else if(type==PT_TABLE)//����ͼ����Ϣ
	{
		for(i=0;i<26;i++)BKP_WriteBackupRegister(BKP_DR11+i*4,Cur_Table[i]);//���浱ǰͼ����Ϣ	  
	} 
}
//��ȡ��ǰ��Ϸ��Ϣ
//��ַ���䣺412~443 
void Read_PTG_Info(u8 type)
{			 
	u8 i;
	if(type==PT_BASIC)//��ȡ������Ϣ
	{										
		PT_Size=BKP_ReadBackupRegister(BKP_DR7);//��ȡͼƬ��С
		PT_New=BKP_ReadBackupRegister(BKP_DR8); //��ȡ�Ƿ��Ѿ�����
		//��ȡ��ǰ���ò���	    
		PT_Step=BKP_ReadBackupRegister(BKP_DR9);	 	   
		//��ȡ��ǰ����ʱ��
		PT_Time=BKP_ReadBackupRegister(BKP_DR10);		   
	}else if(type==PT_TABLE)//����ͼ����Ϣ
	{
		for(i=0;i<26;i++)Cur_Table[i]=BKP_ReadBackupRegister(BKP_DR11+i*4);//��ȡ��ǰͼ����Ϣ	  
	}
}
//�������ֵ
//level:�ȼ� 0~2 
// step:����
// time:��ʱ
//��ַ����:444~	455
void Save_PTG_Best(u8 level,u16 step,u16 time)
{		
	//���浱ǰ���ò���
	BKP_WriteBackupRegister(BKP_DR37+level*8,step); 	 		
	//���浱ǰ����ʱ��
	BKP_WriteBackupRegister(BKP_DR38+level*8,time);	   	
}
//��ȡ���ֵ
//level:�ȼ� 0~2 == 3 4 5 
// step:����
// time:��ʱ
//��ַ����:444~	455
void Read_PTG_Best(u8 level,u16 *step,u16 *time)
{				
	//��ȡ��ǰ���ò���	
	*step=BKP_ReadBackupRegister(BKP_DR37+level*8); 	 
	//��ȡ��ǰ����ʱ��
	*time=BKP_ReadBackupRegister(BKP_DR38+level*8); 			
}

//��ָ���б��õ�ͼƬ��ʼ�����
//���������LCD��ʾ�й�
//table:�̶�����ֵ 0~PT_Size*PT_Size
//xpos :x����ֵ
//ypos :y����ֵ	
//CHECK OK 09/05/30			 
void Get_Ps_Addr(u8 table,u16 *xpos,u16 *ypos)
{
	u8 fw;
	if(PT_Size==3) fw=6;
	else if(PT_Size==4) fw=5;
	else fw=5;						 

	if(table<PT_Size*PT_Size)
	{
		*xpos=Xscal*(table%PT_Size)+fw;
		*ypos=Xscal*(table/PT_Size)+fw;
	}
	else//��PT_Size*PT_Size�����ʼ��ַ
	{
		*xpos=Xscal*(PT_Size-1)+fw;
		*ypos=Xscal*PT_Size+fw;
	}
}
/////////////////////////////////////////////////////////////////////
//���ƴͼ��Ϸ��ʤ���㷨
//����ֵ��0��û�ɹ���1���ɹ���
//֧�ִ�3*3~15*15�����̴�С
//CHECK OK 09/05/30
u8 PT_Win_Chk(void)
{
	u8 t;
	u8 size;
	size=PT_Size*PT_Size; 
	for(t=0;t<size;t++){if(Cur_Table[t]!=t)break;}    
	if(t==size)return 1;
	return 0;
}
//��ʼ��(�������λ��)	 
//�����ƴͼ�ܷ���ɵ��㷨
//CHECK OK 09/05/30
void PT_Map_Init(void)
{		 
	u8 size;		 
	u8 i,t,a, b;	 
	u8 d=1;
	signed char w,h;	 
	size=PT_Size*PT_Size;	 								  
	srand(RTC->CNTL);//�õ�����	   
	for(i=0;i<size;i++)Cur_Table[i]=i;	  
	Cur_Table[size]=size-1;  //���һ��ͼƬ���������ԭλ��������Ϸ�޷���� 
	// ��ʼ��(�������λ��)	 	   
	i=0;
	while(i++<size||d==0)// �������ٵ�λ������
	{
		do 	
		{										  
			a = rand()%(size);			   
			b = rand()%(size);
		}while(a==b);   
		t=Cur_Table[a];
		Cur_Table[a]=Cur_Table[b];
		Cur_Table[b]=t;
		// ������룬����ҪģΪ1�н�
		if(Cur_Table[a]==(size-1)|| Cur_Table[b]==(size-1))
		{
			w=a%PT_Size-b%PT_Size+1;
			h=a/PT_Size-b/PT_Size;
			w=w<0?-w:w;
			h=h<0?-h:h;
			d=(d+w+h)%2;
		}else d=!d;	 
	}	 
	for(i=0;i<size;i++)
	{
		if(Cur_Table[i]==size-1){Cur_Table[i]=size;break;}	   
	}
}

//װ����ϷͼƬ
//֧�ִ�3*3~15*15�����̴�С
//CHECK OK 09/05/30
void Load_Game_PIC(void) 
{
	FileInfoStruct TempPic;//����ͼƬ����ʱ��ſռ�	    
	u8 t;
	u8 temp;
	u16 tempx,tempy; 
	u8 size;
	size=PT_Size*PT_Size;   									  
	TempPic.F_StartCluster=Pic_Addr[0];//����Ԥ��ͼƬ		   
	TempPic.F_Type=T_BMP;//ָ��ΪBMPͼƬ
	AI_LoadPicFile(&TempPic,3,341,104,442);//װ��Ԥ��ͼƬ 
	//����	  
	for(t=0;t<size+1;t++)
	{
		temp=Cur_Table[t];
		Get_Ps_Addr(t,&tempx,&tempy);   
		if(temp==size)//�ո�������������ɫ	
		{
		 	LCD_Fill(tempx,tempy,tempx+Xscal-2,tempy+Xscal-2,LGRAY);	 
		}else
		{
			TempPic.F_StartCluster=Pic_Addr[temp+1];//ͼƬ��ַ		   
			AI_LoadPicFile(&TempPic,tempx,tempy,tempx+Xscal-2,tempy+Xscal-2);	 
		}
	}	 
}
//sour:���������Ĺ̶����
//dest:�ո����ڵĹ̶����
//�ո�һ������dest������
//CHECK OK 09/05/30
void Move_Pic(u8 sour,u8 dest)
{
	FileInfoStruct PicPart;//����ͼƬ����ʱ��ſռ�	    
	u8 pic_real_addr;
	u16 tempx,tempy;			 

	pic_real_addr=Cur_Table[sour]+1;//�õ�ͼƬ�ı�ŵ�ַ 
	PicPart.F_StartCluster=Pic_Addr[pic_real_addr];
	PicPart.F_Type=T_BMP;//ָ��ΪBMPͼƬ	
	//���sour����ͼƬ�����ǰѿո��Ƶ������
	Get_Ps_Addr(sour,&tempx,&tempy);//�õ�Դ����   
	LCD_Fill(tempx,tempy,tempx+Xscal-2,tempy+Xscal-2,LGRAY);											    	 
	//��sour����ͼƬ������ʾ��dest��
	Get_Ps_Addr(dest,&tempx,&tempy);//�õ�Ŀ������
	AI_LoadPicFile(&PicPart,tempx,tempy,tempx+Xscal-2,tempy+Xscal-2);												 
	Cur_Table[dest]=Cur_Table[sour];//��sour����ͼƬ��Ÿ���dest��
	Cur_Table[sour]=PT_Size*PT_Size;//�ո���sour���ˣ�������� 	
}   
	    
//����ֵ��PT_Size*PT_Size+1������Ҫ�ƶ�
//0~PT_Size*PT_Size ��Ҫ�ƶ�����key����
//ƴͼ��Ϸ�ĺ����㷨		   
//֧�ִ�3*3~15*15�����̴�С
//CHECK OK 09/05/30
u8 Need_Move(u8 key)
{
	u8 temp;
	u8 size;
	size=PT_Size*PT_Size;	  
	if(Cur_Table[key]==size)return size+1;//�����˿ո񣬲��ƶ�
	else 
	{
		//////////////////////////�ж�key���Ϸ�////////////////////	 
		if(key>=PT_Size)//��һ�е��Ϸ������жϣ���Ϊ�Ϸ�û����
		{
			if(key==size)temp=size-1;//������������һ���������Ϸ����ֻ���һ
			else temp=key-PT_Size;	//�õ�key���Ϸ����
			if(Cur_Table[temp]==size)return temp;//key���Ϸ��ǿո񣬷��ؿո����ڵ�ַ
		}	
		//////////////////////////�ж�key���·�////////////////////     
		if(key<(size-PT_Size))//���һ�е��·������жϣ���Ϊ�·�û����
		{
			temp=key+PT_Size;  //�õ�key�·����
			if(Cur_Table[temp]==size)return temp;//key���·��ǿո񣬷��ؿո����ڵ�ַ
		}    
		//////////////////////////�ж�key����//////////////////// 
		if(key%PT_Size!=0)//�󷽵ĵ�һ�в����жϣ���Ϊ��û����
		{
			temp=key-1;	//�õ�key�󷽵ı��		    
			if(Cur_Table[temp]==size)return temp;//key�����ǿո񣬷��ؿո����ڵ�ַ
		}	 
		//////////////////////////�ж�key���ҷ�////////////////////
		if((key+1)%PT_Size!=0 || (key+1)==size)//�ҷ������һ�в����жϣ���Ϊ�ҷ�û����
		{									   //�����ڶ������������ҷ������һ�У������һ���൱�������ҷ�����Ҳ��Ҫ�ж�
			temp=key+1;//�õ�key�ҷ��ı��		 
		    if(temp==(size+1))return size+1;//�����е������һ�����ҷ�û���ˣ������ƶ�  
			if(Cur_Table[temp]==size)return temp;//key���ҷ��ǿո񣬷��ؿո����ڵ�ַ
		}	 
		return size+1;//����Ҫ�ƶ�
	}
}
/////////////////////////////////////////////////////////////////////
//ƴͼ��Ϸ�İ���ֵ��ȡ����
//���ذ���ֵ 		 
u8 PTGame_Key_To_Num(void)
{
	u8 t,ytemp;	
	u8 fw;					   
	if(Is_In_Area(130,420,40,24))return 0XFF;//ѡ�����˳��������˳�ģʽ		 
	if(PT_Size==3) fw=6;
	else if(PT_Size==4) fw=5;
	else fw=5;
	for(t=0;t<PT_Size;t++)//���س��水��
	{	  		   	
		for(ytemp=0;ytemp<PT_Size;ytemp++)
		if(Is_In_Area(fw+Xscal*t,fw+Xscal*ytemp,Xscal,Xscal))return t+ytemp*PT_Size+1;
	}    
	if(PT_Size==3&&Is_In_Area(212,315,Xscal,Xscal))return 10;
	else if(PT_Size==4&&Is_In_Area(239,317,Xscal,Xscal))return 17;
	else if(PT_Size==5&&Is_In_Area(253,315,Xscal,Xscal))return 26;
	return 0;//������Ч
} 				    
//��ʾ����
//��ʾ�����ʱ��	 
void PTGame_Show_Msg(void)
{
	POINT_COLOR=RED;//������ɫ
	BACK_COLOR=LGRAY;	   
	LCD_ShowNum(160,370,PT_Step,4,16,0x00);//��ʾ����
	LCD_ShowNum(160,390,PT_Time,4,16,0x00);//��ʾʱ��	  		  
}
//ƴͼ��Ϸʤ��ʱ�Ľ���
//�����Ƿ񴴼�¼���    
void PTG_Win_Msg(void)
{		   
	u8 BEST=0;
	u16 timex,stepx;  
	u16 minx=0;	 
	u8 tkey;  
	//��ʼ�ж��ǲ��Ǵ���¼��
	Read_PTG_Best(PT_Size-3,&stepx,&timex);//��ȡ�ü�������¼	
	if(timex==0)BEST=1;				       //��һ��
	if(timex<stepx)minx=timex;			   //ȥ��С��ֵ
	else minx=stepx;      
	if(PT_Time<minx||PT_Step<minx)BEST=1;//ʱ��/����С����ǰ���ֵ����¼����  
	else if(PT_Time==timex&&PT_Step<stepx)BEST=1;//ʱ����ȣ��������٣�Ҳ�Ƽ�¼
	 
	LCD_Fill(80,120,240,200,GRAY);//���հ���
	POINT_COLOR=BROWN;
	LCD_DrawRect(79,119,241,201);//���յķ���
	LCD_DrawRect(80,120,240,200);//���յķ���
	LCD_DrawRect(81,121,239,199);//���յķ���	 
	BACK_COLOR=GRAY;
	POINT_COLOR=RED;
	if(BEST)
	{	
		Save_PTG_Best(PT_Size-3,PT_Step,PT_Time);//������Ѽ�¼
		Show_Str_Mid(80,122,"����¼�ˣ�",16,160);//����ģʽ,���Զ�����						 
	}
	else Show_Str_Mid(80,122,"��ϲ���أ�",16,160);//����ģʽ,���Զ�����
	POINT_COLOR=BLACK;	
	Show_Str(84,140,"���ĳɼ���",16,0X01);//����ģʽ,���Զ�����
	Show_Str(84,158,"����:",16,0X01);//����ģʽ,���Զ�����	  
	Show_Str(164,158,"ʱ��:",16,0X01);//����ģʽ,���Զ�����
	POINT_COLOR=RED;
	LCD_ShowNum(124,158,PT_Step,4,16,0x00);//��ʾ����
	LCD_ShowNum(204,158,PT_Time,4,16,0x00);//��ʾʱ��   
	BACK_COLOR=LGRAY;
    Show_Str(144,179,"ȷ��",16,0X00);//����ģʽ,���Զ�����		   
	POINT_COLOR=BROWN;	   
	LCD_DrawRect(143,178,176,195);//���յķ���
	
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ��� 
	timex=0;
	while(1)//�ȴ������˳�
	{
		tkey=AI_Read_TP(30,10);
		if(tkey)Pen_Point.Key_Reg|=0X80;//��ǰ����Ѿ������� 	   
		else if(Pen_Point.Key_Reg&0X80)//�����ɿ���
		{
			Pen_Point.Key_Reg=0;
  			if(Is_In_Area(144,179,32,16))return;//ȷ������������  

		}
		Delay_ms(10);   
	}
}	
//��ʾ��Ѽ�¼
void PTG_Best_Show(void)
{	
	u8 temp;
	u16 timex,stepx;

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
	//��ʾ����
	POINT_COLOR=WHITE;
	Show_Str_Mid(0,2,"��Ѽ�¼",16,320); //����ģʽ,���Զ����� 
	Show_Str(5,460,"ȷ��",16,0x01);   //����ģʽ,���Զ�����
    //��ʾ��Ϣ	   
	POINT_COLOR=RED;
	Show_Str_Mid(0,50,"��",16,320);   
	Show_Str_Mid(0,114,"һ��",16,320); 
	Show_Str_Mid(0,178,"����",16,320); 
		  
	LCD_Fill(0,74,319,106,LGRAY);   //�����ɫ
	LCD_Fill(0,138,319,170,LGRAY);  //�����ɫ
	LCD_Fill(0,202,319,234,LGRAY);  //�����ɫ
	
	BACK_COLOR=LGRAY;//�ٶ�û��ѡ�У�ԭ���� 
	POINT_COLOR=DARKBLUE;    
	for(temp=0;temp<3;temp++)
	{
		Show_Str(52,82+temp*64,"����:",16,0x01);   //����ģʽ,���Զ����� 
		Show_Str(176,82+temp*64,"ʱ��:",16,0x01);  //����ģʽ,���Զ����� 
	}
	POINT_COLOR=RED; 
	for(temp=0;temp<3;temp++)
	{
		Read_PTG_Best(temp,&stepx,&timex);//��ȡ���ֵ	 
		LCD_ShowNum(92,82+temp*64,stepx,4,16,0x00);//��ʾ����
		LCD_ShowNum(216,82+temp*64,timex,4,16,0x00);//��ʾʱ�� 	   
	}
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ��� 
	temp=0;
	while(1)//�ȴ������˳�
	{
		temp=AI_Read_TP(30,10);
		if(temp)Pen_Point.Key_Reg|=0X80;//��ǰ����Ѿ������� 	   
		else if(Pen_Point.Key_Reg&0X80)//�����ɿ���
		{
			Pen_Point.Key_Reg=0;
  			if(Is_In_Area(0,456,40,24))return;//ȷ������������  

		}
		Delay_ms(10);   	    
	}	  						   	   
}
extern u8 TimeDisplay;   
//��ƴͼ��Ϸ									   
void PTGame_Play(void)
{
	u8 key,temp;
	u8 size;		  		    

	//���ȵõ���Ϸ��ͼƬ�ļ�
	temp=SysInfoGet(0x80+PT_Size);//����ͼƬ
	if(temp)
	{
	    LCD_Clear(WHITE);//����  		  
		POINT_COLOR=RED;
		Show_Str_Mid(0,160,"ƴͼ��Ϸ���ݶ�ʧ",16,320); //����ģʽ,���Զ�����
		Show_Str_Mid(0,180,"����ϵͳ�ļ��������ԣ�",16,320); //����ģʽ,���Զ�����
		Delay_ms(1500);
		return;
	}			    		 
	//��һ��Ҫ�õ���������
	if(PT_Size==3)Xscal=103;
	else if(PT_Size==4)Xscal=78;
	else if(PT_Size==5)Xscal=62;    
	size=PT_Size*PT_Size; 
	if(PT_New)
	{
		PT_Time=0;//ʱ�����
		PT_Step=0;//�������		
		PT_Map_Init();//��Ҫ���´���ͼƬ
	}else Read_PTG_Info(PT_TABLE);//��ȡ�б���Ϣ		 

	Load_Game_ICO(PT_Size);//װ��X*X����
	Load_Game_PIC();//װ��ͼƬ
	//�õ���Ϸ��ʼʱ��
	key=0;		  
	PTGame_Show_Msg();//��ʾʱ�䣬����  
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
	RTC_ClearITPendingBit(RTC_IT_SEC);
	RTC_ITConfig(RTC_IT_SEC,ENABLE);
	TimeDisplay=0; 
	while(1)//�ȴ�����
	{
		key=AI_Read_TP(30,1);	    
		if(key)Pen_Point.Key_Reg|=0X80;  //��ǰ����Ѿ�������(�����ǻ������Ƕ���) 	   
		else if(Pen_Point.Key_Reg&0X80)//�����ɿ���
		{
			Pen_Point.Key_Reg=0;		  	
			key=PTGame_Key_To_Num(); 		   	   
			if(key==0XFF)
			{
				PT_New=0;//��Ϸû�н������´ο��Բ�����
				Save_PTG_Info(PT_BASIC);//���������Ϣ
				Save_PTG_Info(PT_TABLE);//�����б���Ϣ
				break;//ѡ�����˳�	 
			}else
			{
				key-=1;//����ֵΪ1~10������1��ƥ��
				temp=Need_Move(key);//����1		  
				if(temp<=size)
				{
					Move_Pic(key,temp);//��key�����ͼƬ��������temp����
					PT_Step++;         //��������
					PTGame_Show_Msg(); //��ʾʱ�䣬����   
					if(PT_Win_Chk())//��Ϸ������ƴͼ�ɹ���
					{								 	 
						PT_New=1;//��Ϸ�������´α������
						Save_PTG_Info(PT_BASIC);//���������Ϣ
						PTG_Win_Msg();//�ɹ�ʱ�Ľ�����ʵ
						break; 
					}
				}
			}
			key=0;
		}
		if(TimeDisplay)//����������1s
		{
			TimeDisplay=0;
			PT_Time++;//ʱ������
			SignalLED=!SignalLED;//LED��˸
			PTGame_Show_Msg();//��ʾʱ�䣬����
		}
	}
	RTC_ITConfig(RTC_IT_SEC,DISABLE);
	RTC_ClearITPendingBit(RTC_IT_SEC);	 
}	    
//��Ϸ ����
void Game_Play(void)
{			    
	u8 selx=0;	 
	while(1)
	{
		selx=Load_Fun_Sel(PTGmenu_F1,"��Ϸѡ��","ȷ��",1,0);
		if(selx==13)return;//����
		else if(selx==0)//ѡ����ƴͼ��Ϸ����ʱֻ����һ����Ϸ
		{
		 	Read_PTG_Info(PT_BASIC);//��ȡ������Ϣ
			if(PT_Size>5||PT_Size<3)//��Ϣ�Ƿ�,���±�����Ϣ
			{
				PT_Size=3;PT_New=0;
				PT_Step=0;PT_Time=0;
				Save_PTG_Info(PT_BASIC);//���������Ϣ	   
				for(selx=0;selx<26;selx++)Cur_Table[selx]=0;
				Save_PTG_Info(PT_TABLE);//����ͼ����Ϣ
				Save_PTG_Best(0,0,0);
				Save_PTG_Best(1,0,0);
				Save_PTG_Best(2,0,0);
			}else if(!PT_New)Read_PTG_Info(PT_TABLE);//��ȡͼ����Ϣ

			while(1)
			{	    
				if(PT_New)selx=Load_Fun_Sel(PTGmenu_S1+1,"ƴͼ��Ϸ","ȷ��",3,0);//����ʾ������ť
				else selx=Load_Fun_Sel(PTGmenu_S1,"ƴͼ��Ϸ","ȷ��",4,0);//��ʾ������ť
				if(selx==13)break;//������һ��Ŀ¼
				if(PT_New)selx+=1;//ƫ��һ��
				switch(selx)
				{
					case 0://ѡ���˼���
						PTGame_Play();
						break;
					case 1://ѡ��������Ϸ
						PT_New=1;//
						PTGame_Play();
						break;
					case 2://�������
						PTG_Best_Show();
						break;
					case 3://�Ѷ�����
						selx=Load_Fun_Sel(PTGmenu_S2,"�Ѷ�ѡ��","ȷ��",3,PT_Size-3);//ѡ���Ѷ�
						if(selx<3)//ѡ������Ч����
						{
							PT_Size=3+selx; 				
							PT_New=1;//����Ҫ����������� 
						}
						break;
				}
			}
		}	 
	} 
}






























