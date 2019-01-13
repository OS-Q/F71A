#include "fat.h"
#include "gui.h"
#include "lcd.h"
#include "flash.h"
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "vs1003.h"
#include "jpegbmp.h"
#include "sdcard.h"
#include "mp3player.h"
  

u16 f_kbps=0;//�����ļ�λ��	

//������,ʱ��,������ ����ʵ��Ϣ��ʾ����
void MP3_PROG(u32 pos,u32 lenth)
{	
	static u16 playtime;//����ʱ����
	static u16 alltime;	     
	u32 propos;	
	u16 time;	// ʱ�����
	POINT_COLOR=0x37E6;	

	if(lenth)
	{		
		propos=(pos*1000)/(lenth>>9); //�õ��ٷֱ� 	
		if(propos>1000)propos=1000;//����propos�ķ�Χ
	}else return;//�ļ�Ϊ��,����
	 
	TFT_ProBar((u16)(propos*280/1000));//��ʾ������  
	
	if(f_kbps==0xffff)//δ���¹�
	{
		playtime=0;
		f_kbps=GetHeadInfo();	   //��ñ�����
		if(f_kbps) alltime=(lenth/f_kbps)/125; //��ʱ��
		else alltime=0;   //�Ƿ�������

		LCD_DisplayChar(189,206,':',12,0x10); //����ʱ��ļ������:��
		LCD_DisplayChar(207,206,'/',12,0x10);//����ʱ������ʱ��ļ����/��
		//��ʾ��ʱ��
		LCD_Show2Num(213,206,alltime/60,12,0x10);//��ʾ�ܷ�����
		LCD_DisplayChar(225,206,':',12,0x10); //��ʱ��ļ������:�� 	  
		LCD_Show2Num(231,206,alltime%60,12,0x10);//��ʾ��������  
		//��ʾλ��			   
		LCD_Show3Num(258,206,f_kbps,12,0x10);
		LCD_DisplayChar(276,206,'k',12,0x10);
		LCD_DisplayChar(282,206,'b',12,0x10);
		LCD_DisplayChar(288,206,'p',12,0x10);
		LCD_DisplayChar(294,206,'s',12,0x10);
	}
	      
  	if(f_kbps)//��λ�ʲ�Ϊ0 ���ļ�
	time=propos*alltime/1000;  //�õ���ǰ����ʱ��
	else time=GetDecodeTime(); //��λ��Ϊ0���ļ�,�õ�����ʱ��
 			  
	if(time!=playtime)//1sʱ�䵽,������ʾ����
	{
		playtime=time;//����ʱ�� 	 		 
		//��ʾ����ʱ��			 
		LCD_Show2Num(177,206,time/60,12,0x10); //����	   		   
		LCD_Show2Num(195,206,time%60,12,0x10);//���� 	 				
	}   		 
}
/*
u8 *MP3_VOL[5]={" ���� "," ���� "," ���� ","������","������"};
u8 *MP3_OTHER[3]={"Ƶ����ʾ","�����ʾ","�˳�����"};
 		  
//������ݺϷ���
void VOL_Data_Check(u8 volx,u8 *datax)
{
	switch(volx)
	{
		case 0:	//������
			if(*datax>64)*datax=64;
			if(*datax<1)*datax=1;
			break;
		case 1:	//����			  
		case 2:	//����
			if(*datax>15)*datax=15;
			if(*datax<1)*datax=1;
			break;
		case 3: //������
		case 4:	//������
			if(*datax>32)*datax=32;
			if(*datax<1)*datax=1;
			break;
		case 5://������
			if(*datax>1)*datax=1;
			break;		    
	}
}*/
/*
//��ʾ����
void MP3_MsgBox_Data(u16 *sysmsg,u8 menu,u8 key)
{
	u16 volx;	   
	BACK_COLOR=MP3COLOR6;//����ɫ
	POINT_COLOR=RED;
	LCD_Fill(58,259,138,275,MP3COLOR6);//Ĩ���ϴ�����
	switch(menu)
	{
		case 0://��Чģʽ		 
			if(key==7) INITDSP++;	 
			else if(key==8)
			{
				if(INITDSP==0)INITDSP=4;//���ݺϷ����ж�
				else INITDSP--; 
			}					    	
			if(INITDSP>4)INITDSP=0;				   
			LCD_Show2Num(82,259,INITDSP+1,16,0);//��ʾ����
			break;
		case 1:
			volx=(*sysmsg>>2)&0x07;
			if(key==7)voltemp[volx]++;	 
			else if(key==8)voltemp[volx]--; 
			VOL_Data_Check(volx,&voltemp[volx]);//���ݺϷ����ж�  
			LCD_Show2Num(82,259,voltemp[volx],16,0);//��ʾ����
			break;
		case 2:	
			volx=(*sysmsg>>5)&0x03;	 
			if(key==7){volx++;if(volx>2)volx=0;}	 
			else if(key==8) 
			{
				if(volx)volx--;
				else volx=2;
			} 				    						    	 
			*sysmsg&=0XFF9F;
			*sysmsg|=volx<<5; 					   
			Show_Str(58,259,MP3_OTHER[volx],16,0);
			break;   
	}
	//��������Ϣ����
	BACK_COLOR=MP3COLOR3;//����ɫ
	POINT_COLOR=GRED;
	for(volx=0;volx<3;volx++)LCD_Show2Num(26+60*volx,47,voltemp[volx],16,0);//��ʾ������Ϣ  
	LCD_Show2Num(206,47,INITDSP+1,16,0);//��ʾ��Ч��Ϣ  
}
*/

//��ʾMP3Ŀ¼�����Ϣ
//*menu	  :��ǰ��Ŀ¼��
//*sysmsg :��ǰ���б�־��Ϣ
//curmenu:��ǰĿ¼��   
/*
void MP3_MsgBox_Menu(u16 *sysmsg,u8 *menu,u8 curmenu)
{
	u16 temp;   	 
	BACK_COLOR=MP3COLOR6;  
	//��ʾ��Ч����
	temp=(*sysmsg>>9)&0x01;
	if(curmenu==0)
	{
		POINT_COLOR=RED;  
		if(*menu==curmenu)//�ٴε��
		{
			temp=!temp;								  
			*sysmsg&=0XFDFF;
			*sysmsg|=temp<<9;
		}
		*menu=curmenu;
	}	 						
	else POINT_COLOR=GREEN;	  
	if(temp)Show_Str_Mid(8,227,"����Ч",16,48);
	else Show_Str_Mid(8,227," ��Ч ",16,48); 
	//��ʾ����
	temp=(*sysmsg>>2)&0x07;	   
	if(curmenu==1)
	{
		POINT_COLOR=RED;
		if(*menu==curmenu)//�ٴε��
		{
			temp++;
			if(temp>=5)temp=0;						   	 
			*sysmsg&=0XFFE3;
			*sysmsg|=temp<<2;
			temp=(*sysmsg>>2)&0x07;
		}
		*menu=curmenu;
	}	 
	else POINT_COLOR=GREEN;	  
	Show_Str_Mid(64,227,MP3_VOL[temp],16,48);
	//����ѡ��			    	 
	if(curmenu==2)
	{
		*menu=curmenu;
		POINT_COLOR=RED;  	 
	}
	else POINT_COLOR=GREEN;	 
	Show_Str_Mid(120,227," ���� ",16,48);	 
}*/
							  	 
//����ѡ���ĸ��� 					  
//index:��ǰ���ŵĸ�������
//total:�ܹ������ļ��ĸ���
//�������ֵļ������п��ƶ����������
u8 Mus_Play(FileInfoStruct *FileName,u16 index,u16 total)
{	 
    unsigned long bfactor; //��������,���Ե�ַ
    unsigned long bcluster;//����غ�
	u32 file_pos;          //��¼�ļ����ŵĳ���
	u8 forpreclust;//���������Ҫ�Ĵ��� 
    u8 count; 		//����������,���ڻ���

 	u8 *BUFFER=(u8*)MCUBuffer;//ָ��JPEG�Ŀ�ʼ512�ֽ�
	u8 *spedate=(u8*)MCUBuffer+512;//��ǰƵ�׷����Ľ���Ĵ���
  	u8 *topval=(u8*)MCUBuffer+526; //FFT������ߵ��¼��
	u8 *flyval=(u8*)MCUBuffer+540; //FFT�����������¼��
	u32 topstay1;   //��ߵ�Ƶ����ʱ1
	u32 topstay2;	//��ߵ�Ƶ����ʱ2
			  	   
	u8 load=0;	 //������ر�ǣ��������Ÿ���ʱ������Ҫÿ���ؼ��ؽ���
	u8 mark=1;	//����FFT��ʾ/��ȡFFT���ݱ��
				//��Ϊ��ȡFFT��VS1003�ѽ������״̬����Ҫ������������
				//������ʾ����һ��ѭ������	
	u8 timer;	//Ƶ�׶�ʱ��
	u16 temp;	//�ݴ�Ĵ���
	u16 i,n;	//ѭ��������
	u8 t,key,keycnt;	//����������
	u8 FFT_LRC=0;//Ĭ��FFT
	//bit0~9������ǹ��� ,Ĭ��ȫ��ѭ��
	//bit0,1:ѭ��ģʽ;bit2,3,4,5,6:��������;bit7:EQ��Ч���;
	//bit8:�Ƿ��и��;bit9:��Ч��/ȡ���;bit10:����/��ͣ���
	u16 sysfun=1;		    									 
 	 		   															 
/*	timer3_init();//��ʼ�������ʾ��ʱ��*/
	Read_VS_Set();//��ȡVS1003������
	Mp3Reset();       //Ӳ��λ
	Vs1003SoftReset();//��λVS1003
	LoadPatch();      //����Ƶ�׷������� 
 	Load_Gui_Mp3(load);//���ز��Ž���
	TFT_Volume(GetVolume());

RESTART:
	if(load)
	{
		Vs1003SoftReset();//��λVS1003
		LoadPatch();     //����Ƶ�׷�������
		Load_Gui_Mp3(load);
	}
	else load=1;	
	//fft����,�������
	topstay1=0;
	topstay2=0;
	file_pos=0;	  
	for(i=0;i<14;i++)//����������
	{
		topval[i]=0;		 	    
		flyval[i]=0;
		spedate[i]=0;
	}		//FFT��ʾ��������						  
	key=0;t=0;
	keycnt=0;
	f_kbps=0xffff;//�ļ�λ������Ϊ������ֵ
	timer=5;	//���õ�һ�������ܵ�ֵ,��ʼ����Ϣ��	
	count=0;	//����������

	FAT_TAB.Fat_Head_Pos=0;
	Copy_Fat_Table(FileName->F_StartCluster); 

    bfactor=fatClustToSect(FileName->F_StartCluster);//�õ���ʼ�ض�Ӧ������
	bcluster=FileName->F_StartCluster;		//�õ��ļ���ʼ�غ�	
	
	forpreclust=FileName->F_Size/(SectorsPerClust*BytesPerSector*50);//ÿ��ǰ��/���� 2%����Ҫ�Ĵ���	   
	if(forpreclust<2)forpreclust=2;//����ҲҪ������
	  
	//�ӵ�ǰĿ¼���Ҹ���ļ�,��,����.	 
/*	if(FindLrcFile(Cur_Dir_Cluster,FileName->F_ShortName))
	{								   
		sysfun|=1<<8;     //����и��
		Lrc_Read_Init();  //��ʼ����ʶ�ȡ
	}else */
	sysfun&=~(1<<8);//���û�и��	 							 					 
	//��ʾ��ǰ�ļ����µĸ�����Ŀ,����ǰ����������
	POINT_COLOR=GradualColor(0xFFFF,0x0000,200); 
	LCD_Show3Num(246,34,index,16,1);
	LCD_DisplayChar(270,34,'/',16,1); 
	LCD_Show3Num(278,34,total,16,1); 
	//��ʾ��������
	Show_Str(15,34,FileName->F_Name,16,1); //��ʾ��������
	//FFT�������			 	    
	Pen_Point.Key_Sta=Key_Up;//�ͷŰ���
				
	while(1)   //�������ֵ���ѭ��
    {
        if(SD_ReadBlock(bfactor<<9,(u32 *)BUFFER,512) != SD_OK)break;//��ȡһ������������ 
        i=0;  	  
        count++;//���������� 
		do      //������ѭ��
        {  
			while(VS_DREQ==0&&((sysfun&0X0400)==0)) //�ȴ���/����Ƶ����ʾ/����ͣ 
			{
				if(FFT_LRC==0)//Ƶ��ģʽ
				{
					if(mark)
					{
						GetSpec(spedate);//�õ�Ƶ������
						MP3_PROG(file_pos,FileName->F_Size);//MP3������ʾ/����ʱ��  
					}
					else TFT_ShowFFT(spedate,topval,flyval,&topstay1,&topstay2);//Ƶ����ʾ     	  
					timer++; 
					if(timer>=3)//FFT��ߵ���������� 
					{
						Top_Delet_FFT(flyval,&topstay1,&topstay2);//ɾ���߶�
						timer=0; 
					}   
				}else 	    //���ģʽ
				{					   											    
				/*	if(mark&&sysfun&(1<<8))LyricDisplayCtrl();//���ڸ��,����ʾ
					else if(mark==0)MP3_PROG(file_pos,FileName->F_Size);//MP3������ʾ/����ʱ�� 				  
					//printf("end\n"); */
				}	
				mark=!mark;
			}	 
			if(VS_DREQ!=0&&((sysfun&0X0400)==0))  //����ͣ,�����ݸ�VS1003
			{			   	 
	            for(n=0;n<32;n++)Vs1003_DATA_Write(BUFFER[i++]);//������������    
			}
			key=AI_Read_TP(1,1);	    
			if(key) t=Touch_To_Num(3);
			else Pen_Point.Key_Reg=0;	   		 
			
			if(t)//�а�������
			{
				switch(t)
				{
					case 1://ѭ��ģʽ����
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							temp=sysfun&0x03;
							temp++;
							if(temp>1)temp=0;//ֻ����������,����ѭ��/ȫ��ѭ��
							//��ʾѭ��ģʽ
							if(temp)
							TFT_FucButton(280,270,30,0x632E,0x2105,"A");
							else
							TFT_FucButton(280,270,30,0x632E,0x2105,"S");		   
							sysfun&=0XFFFC;
							sysfun|=temp;//���ı�־λ
						}
						Pen_Point.Key_Reg|=0X80; //��ǰ����Ѿ�����	
						break;
					case 2://EQ ����
						if((Pen_Point.Key_Reg&0x80)==0)
						{
						}
						Pen_Point.Key_Reg|=0X80; //��ǰ����Ѿ�����
						break;
					case 3://LRC����
						if((Pen_Point.Key_Reg&0x80)==0)
						{
						}
						Pen_Point.Key_Reg|=0X80; //��ǰ����Ѿ�����
						break;    
					case 4://�������� 
						sysfun |= 0x0200;					 	  
						TFT_Volume((Pen_Point.X-VOLBAR_X)/3);
						SetVolume((Pen_Point.X-VOLBAR_X)/3);
						break;
					case 5://�ر�
						if(sysfun&0x0200)
						Save_VS_Set();
						Mp3Reset();
						return 0xFF;
					case 6://��һ��
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							index--;
							if(index==0)index=total;
							Get_File_Info(Cur_Dir_Cluster,FileName,T_MP3|T_WMA|T_WAV|T_MID,&index);
							goto RESTART;
						}
						Pen_Point.Key_Reg|=0X80; //��ǰ����Ѿ�����
						break;					
					case 7://������ͣ
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							temp=(sysfun>>10)&0x01;
							temp=!temp;	  	   
							if(temp) TFT_DrawCButton(100,255,25,0xACF9,0x2910,1);//����
							else TFT_DrawCButton(100,255,25,0xACF9,0x2910,2);//��ͣ						    
							sysfun&=0XFBFF;
							sysfun|=temp<<10;
						}
						Pen_Point.Key_Reg|=0X80; //��ǰ����Ѿ�����
						break;
					case 8://��һ��
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							index++;
							if(index>total)index=1;
							Get_File_Info(Cur_Dir_Cluster,FileName,T_MP3|T_WMA|T_WAV|T_MID,&index);
							goto RESTART;
						}
						Pen_Point.Key_Reg|=0X80; //��ǰ����Ѿ�����	
						break;	 
					case 9://������߿��� 
                        if(keycnt>2)   
                        {	   
							temp=((float)file_pos*512/FileName->F_Size)*280;//�õ��ٷֱ� 
							if(temp+22<Pen_Point.X) //��ǰ����С��ָ��λ��,���
							{
	                            for(i=0;i<forpreclust;i++)//���ƫ��5����
	                            {
	                                bcluster=FatTab_Next_Cluster(bcluster);//ƫ��һ����
	                                if((FAT32_Enable==0&&bcluster==0xffff)||bcluster==0x0ffffff8||bcluster==0x0fffffff) 
	                                {
	                                    bcluster=FatTab_Prev_Cluster(bcluster);//��һ����	   
										break;
	                                }
	                            } 	
								file_pos+=SectorsPerClust*i; //�ļ��α����
							}else if(temp+18>Pen_Point.X) //һ����̫�õĿ����㷨.
							{
	                            for(i=0;i<forpreclust;i++)//��ǰƫ��5����
	                            {
	                                bcluster=FatTab_Prev_Cluster(bcluster);//��ǰһ��
	                                if((FAT32_Enable==0&&bcluster==0xffff)||bcluster==0x0ffffff8||bcluster==0x0fffffff) 
	                                {
	                                    bcluster=FatTab_Next_Cluster(FileName->F_StartCluster);//���һ����	   
										break;
	                                }
	                            }
								if(bcluster==FileName->F_StartCluster) file_pos=0;								
								else file_pos-=SectorsPerClust*i;//�ļ��α�ݼ�						
							}else break;//���趨��Χ��,����Ҫ���� 

							bfactor=fatClustToSect(bcluster);//�õ���ǰӦ���ŵ�����	    
							bfactor--;//Ϊ��������������һ��	  
                            
							count=0; 
							i=512;    //����while��
							MP3_PROG(file_pos,FileName->F_Size);//MP3������ʾ/����ʱ��  	  
                        }else keycnt++;	//�ۻ���������  	  			
						break;  
				}  
				t=0;			  						  
			}  		           
        }while(i<511);//ѭ������512���ֽ� 
		file_pos++;//����512���ֽ�	  
        bfactor++; //������
        if(count>=SectorsPerClust)//һ���ؽ���,����
        {
            count=0;  		   
            bcluster=FatTab_Next_Cluster(bcluster);
			//�ļ����� 
			if((FAT32_Enable==0&&bcluster==0xffff)||bcluster==0x0ffffff8||bcluster==0x0fffffff)break;//error	
            bfactor=fatClustToSect(bcluster);
	    }
		if(file_pos%(forpreclust*SectorsPerClust)==1)
		SignalLED=0;
		else SignalLED=1; 
    } 				   
	temp=sysfun&0x03;//0/1				 
	if(temp==1)//ȫ��ѭ��
	{
		index++; 						   
    	if(index>total)index=1;
		Get_File_Info(Cur_Dir_Cluster,FileName,T_MP3|T_WMA|T_WAV|T_MID,&index);			  
	}							   
	goto RESTART;//���»ص����ﲥ��,����/ȫ��ѭ��	 
}

//���������
//֧��FAT32/16�ļ�ϵͳ�����.
//check:09/04/18
void Mus_Viewer(void)
{		      
	FileInfoStruct *FileInfo;		 
	u16 mus_cnt=0;//��ǰĿ¼�������ļ��ĸ���
	u16 index=0;  //��ǰѡ����ļ����	   
	//�Ӹ�Ŀ¼��ʼ���
	if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//��Ŀ¼�غ�
 	else Cur_Dir_Cluster=0;						   
	Fat_Dir_Cluster=Cur_Dir_Cluster;//��Ŀ¼���ǵ�ǰĿ¼
	while(1)
	{
BROS:	 	     
		Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
		Load_Gui_Bro("ѡ��");//��ʾGUI����
		POINT_COLOR=WHITE;	
		Show_Str(128,2,"�������",16,0x01);//����ģʽ,���Զ�����
			
		index=File_Browse(T_MP3|T_WMA|T_WAV|T_MID);//����ļ���    
		if(!index)return;//�������,�˳��˹���!!!!!

		mus_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_MP3|T_WMA|T_WAV|T_MID,&mus_cnt);//��ȡ��ǰ�ļ��������Ŀ���ļ����� 
		Pen_Point.Key_Sta=Key_Up;// ����ʾ�ڼ�Ĵ��� 
		FileInfo=&F_Info[0];//�����ݴ�ռ�.
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_MP3|T_WMA|T_WAV|T_MID,&index);//�õ����׸�������Ϣ	 	  
			Mus_Play(FileInfo,index,mus_cnt);			   			 	   
			goto BROS;	   
		}
	}			 
}



















