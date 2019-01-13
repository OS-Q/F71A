#include "fontupd.h"
#include "fat.h"
#include "lcd.h"
#include "flash.h"
#include "text.h"
#include "delay.h"
#include "jpegbmp.h"


//Mini STM32������
//���ĺ���֧�ֳ��� V1.1
//�����������,�Լ��ֿ��׵�ַ��ȡ2������.
//����ԭ��@ALIENTEK
//2010/5/23	

//�������ֶ�һ����Ҫ�Ҹ�!!!
//�ֽ�0~3,  ��¼UNI2GBKADDR;�ֽ�4~7  ,��¼UNI2GBKADDR�Ĵ�С
//�ֽ�8~11, ��¼FONT16ADDR ;�ֽ�12~15,��¼FONT16ADDR�Ĵ�С
//�ֽ�16~19,��¼FONT12ADDR ;�ֽ�20~23,��¼FONT12ADDR�Ĵ�С		    
//�ֽ�24,��������ֿ��Ƿ���ڵı�־λ,0XAA,��ʾ�����ֿ�;����ֵ,��ʾ�ֿⲻ����!

extern unsigned char *folder[];	 //ϵͳ�ļ���
extern unsigned char *sysfile[]; //ϵͳͼ��

//�ֽ�0~3,  ��¼UNI2GBKADDR;�ֽ�4~7  ,��¼UNI2GBKADDR�Ĵ�С
//�ֽ�8~11, ��¼FONT16ADDR ;�ֽ�12~15,��¼FONT16ADDR�Ĵ�С
//�ֽ�16~19,��¼FONT12ADDR ;�ֽ�20~23,��¼FONT12ADDR�Ĵ�С
//������,��100K����ʼ
//100K,���UNICODE2GBK��ת����	


//���������ļ�
//����ֵ:0,���³ɹ�;
//		 ����,�������.
//����ԭ��@ALIENTEK
//V1.1
#ifdef EN_UPDATE_FONT
//u8 temp[512];  //��ʱ�ռ�
u8 Update_Font(void)
{
	u32 fcluster=0;	   
	u32 i;
	u32 tempsys[10];  //��ʱ��¼�ļ���ʼλ�ú��ļ���С
	u8 *ftemp=jpg_buffer;
	float prog;
	u8 t=0;		 
	FileInfoStruct FileTemp;//��ʱ�ļ���  
	tempsys[0]=0x00000000; 
	tempsys[1]=0x00000000;				    	 
	//�õ���Ŀ¼�Ĵغ�
	if(FAT32_Enable)fcluster=FirstDirClust;
	else fcluster=0;			   
	FileTemp=F_Search(fcluster,(unsigned char *)folder[0],T_FILE);//����system�ļ���	  
	if(FileTemp.F_StartCluster==0)return 1;	   //ϵͳ�ļ��ж�ʧ	
	else						  		  
	{	 										 
		//�Ȳ�������
		FileTemp=F_Search(FileTemp.F_StartCluster,(unsigned char *)folder[1],T_FILE);//��system�ļ����²���FONT�ļ���
		if(FileTemp.F_StartCluster==0)return 2;//�����ļ��ж�ʧ	
		fcluster=FileTemp.F_StartCluster;      //�����ļ��дغ�	
		    
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[3],T_SYS);//��system�ļ����²���SYS�ļ�
		if(FileTemp.F_StartCluster==0)return 3;//UNI2GBK�ļ���ʧ	 
		LCD_DisplayStringLine(Line3,6,"Write UNI2GBK to FLASH...   %");				
		F_Open(&FileTemp);//�򿪸��ļ�
		i=0; 	  
		while(F_Read(&FileTemp, ftemp))//�ɹ�����512���ֽ�
		{		 
			if(i<FileTemp.F_Size)//�������ļ���С
			{
				SPI_Flash_Write(ftemp,i+UNI2GBKADDR,512);//��100K�ֽڴ���ʼд��512������   
				i+=512;//����512���ֽ�
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(248,48,t,3,16,0x00);//��ʾ��ֵ
			}					    
		}
		LCD_DisplayStringLine(Line4,6,"UNICODE2GBK UPDATE SUCESS!");
		Delay_ms(500);

		tempsys[1]|=0x55000000;
		tempsys[8]=UNI2GBKADDR;			  //UNI2GBK��ַ
		tempsys[9]=FileTemp.F_Size;	 	  //UNI2GBK ��С

		Delay_ms(500);			    
		//printf("UNI2GBKд��FLASH���!\n");
		//printf("д�����ݳ���:%d\n",FileTemp.F_Size);
		//printf("UNI2GBKSADDR:%d\n\n",UNI2GBKADDR);
		
		//FONT16ADDR=FileTemp.F_Size+UNI2GBKADDR;//F16���׵�ַ 
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[0],T_SYS);//��system�ļ����²���FONT16�����ļ�
		if(FileTemp.F_StartCluster==0)return 4;//FONT16�����ļ���ʧ	 
	
		LCD_DisplayStringLine(Line6, 6, "Write FONT16 to FLASH...   %");		
 		//printf("��ʼFONT16д��FLASH...\n");		
		F_Open(&FileTemp);//�򿪸��ļ�
		i=0;	  
		while(F_Read(&FileTemp, ftemp))//�ɹ�����512���ֽ�
		{
			if(i<FileTemp.F_Size)//�������ļ���С
			{
				SPI_Flash_Write(ftemp,i+FONT16ADDR,512);//��0��ʼд��512������   
				i+=512;//����512���ֽ�
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(240,96,t,3,16,0x00);//��ʾ��ֵ
			}
		}
		Show_Str(48, Line7, "16*16�����ֿ�������!", 16, 0);
		Delay_ms(500);
														   
		tempsys[1]|=0x00000016;
		tempsys[2]=FONT16ADDR;
		tempsys[3]=FileTemp.F_Size;	 	  //FONT16ADDR ��С
		Delay_ms(500);
			   
		/*printf("FONT16д��FLASH���!\n");
		printf("д�����ݳ���:%d\n",FileTemp.F_Size);
		FONT12ADDR=FileTemp.F_Size+FONT16ADDR;//F16���׵�ַ
		printf("FONT16SADDR:%d\n\n",FONT16ADDR);
		LCD_ShowString(20,60,"Write FONT12 to FLASH... ");		
		FONT12��ʱ������*/
					  
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[1],T_SYS);//��system�ļ����²���FONT12�����ļ�
		if(FileTemp.F_StartCluster==0)return 5;//FONT12�����ļ���ʧ	 
		LCD_DisplayStringLine(Line9, 6, "Write FONT12 to FLASH...   %");	
		//printf("��ʼFONT12д��FLASH...\n");		
		F_Open(&FileTemp);//�򿪸��ļ�
		i=0;	  
		while(F_Read(&FileTemp, ftemp))//�ɹ�����512���ֽ�
		{
			if(i<FileTemp.F_Size)//�������ļ���С
			{
				SPI_Flash_Write(ftemp,i+FONT12ADDR,512);//��0��ʼд��512������   
				i+=512;				//����512���ֽ�
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(240,144,t,3,16,0x00);//��ʾ��ֵ
			}
		}
		Show_Str(48, Line10, "12*12�����ֿ�������!", 12, 0);
		Delay_ms(500);

		tempsys[1]|=0x00001200;	    
		tempsys[4]=FONT12ADDR;
		tempsys[5]=FileTemp.F_Size;	 	  //FONT12ADDR ��С
		Delay_ms(500);

		/*printf("FONT12д��FLASH���!\n");
		printf("д�����ݳ���:%d\n",FileTemp.F_Size);   
		printf("FONT12SADDR:%d\n\n",FONT12ADDR); */
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[2],T_SYS);//��system�ļ����²���FONT12�����ļ�
		if(FileTemp.F_StartCluster==0)return 6;//FONT24�����ļ���ʧ	 
		LCD_DisplayStringLine(Line12, 6, "Write FONT24 to FLASH...   %");	
		//printf("��ʼFONT12д��FLASH...\n");		
		F_Open(&FileTemp);//�򿪸��ļ�
		i=0;	  
		while(F_Read(&FileTemp, ftemp))//�ɹ�����512���ֽ�
		{
			if(i<FileTemp.F_Size)//�������ļ���С
			{
				SPI_Flash_Write(ftemp,i+FONT24ADDR,512);//��0��ʼд��512������   
				i+=512;				//����512���ֽ�
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(240,192,t,3,16,0x00);//��ʾ��ֵ
			}
		}
		Show_Str(48, Line14, "24*24�����ֿ�������!", 24, 0);
		Delay_ms(500);

		tempsys[1]|=0x00240000;	    
		tempsys[6]=FONT24ADDR;
		tempsys[7]=FileTemp.F_Size;	 	  //FONT24ADDR ��С
		Delay_ms(500);

	}
	tempsys[0]=0x45544F46;		   //д���ֿ���ڱ�־
   	SPI_Flash_Write((u8 *)tempsys,0,40);	
	Show_Str(80,Line17,"�ֿ������ɣ�",24,0);		    
	Delay_ms(3000);
	LCD_Clear(BACK_COLOR);				
	return 0;//�ɹ�
}
#endif

//����������õ������ַ
//��ϵͳʹ�ú���֧��֮ǰ,������øú���
//����FONT16ADDR,FONT12ADDR,UNI2GBKADDR;  
u8 Font_Init(void)
{
	u32 tempsys[10];//��ʱ��¼�ļ���ʼλ�ú��ļ���С

	#ifdef SYSTEM_UPDATE
	SPI_Flash_Erase_Chip();
	#endif

	SPI_Flash_Read((u8 *)tempsys,0,40);		//�����ֿ��־λ,���Ƿ�����ֿ�
	if(tempsys[0]==0x45544F46)	
	{
/*		Show_Str(80,Line10,"�ֿ��Ѿ����ڣ�",12,0);
		Show_Str(80,Line12,"�ֿ��Ѿ����ڣ�",16,0);
		Show_Str(80,Line14,"�ֿ��Ѿ����ڣ�",24,0);
		Delay_ms(2000);	
		LCD_Clear(BACK_COLOR); */
		return 0;	//FONTE ��־�ֿ����
	 }
	else
	{
		if(Update_Font()!=0)
		return 1;
	}	
	return 0;	 
}

/*
void DMA_M2MInit(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)0;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 32;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
}

void DMA_M2MStart(u32 SRCAddr, u32 DSTAddr, u32 num)
{
  DMA1_Channel6->CPAR = SRCAddr + Bank1_SRAM3_ADDR;
  DMA1_Channel6->CMAR = DSTAddr;
  DMA1_Channel6->CNDTR = num;
  DMA_Cmd(DMA1_Channel6, ENABLE);
  while(DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET);
  DMA_ClearFlag(DMA1_FLAG_TC6);
  DMA_Cmd(DMA1_Channel6, DISABLE);
}
*/   




























