#include "fontupd.h"
#include "fat.h"
#include "lcd.h"
#include "flash.h"
#include "text.h"
#include "delay.h"
#include "jpegbmp.h"


//Mini STM32开发板
//中文汉字支持程序 V1.1
//包括字体更新,以及字库首地址获取2个函数.
//正点原子@ALIENTEK
//2010/5/23	

//以下下字段一定不要乱改!!!
//字节0~3,  记录UNI2GBKADDR;字节4~7  ,记录UNI2GBKADDR的大小
//字节8~11, 记录FONT16ADDR ;字节12~15,记录FONT16ADDR的大小
//字节16~19,记录FONT12ADDR ;字节20~23,记录FONT12ADDR的大小		    
//字节24,用来存放字库是否存在的标志位,0XAA,表示存在字库;其他值,表示字库不存在!

extern unsigned char *folder[];	 //系统文件夹
extern unsigned char *sysfile[]; //系统图标

//字节0~3,  记录UNI2GBKADDR;字节4~7  ,记录UNI2GBKADDR的大小
//字节8~11, 记录FONT16ADDR ;字节12~15,记录FONT16ADDR的大小
//字节16~19,记录FONT12ADDR ;字节20~23,记录FONT12ADDR的大小
//字体存放,从100K处开始
//100K,存放UNICODE2GBK的转换码	


//更新字体文件
//返回值:0,更新成功;
//		 其他,错误代码.
//正点原子@ALIENTEK
//V1.1
#ifdef EN_UPDATE_FONT
//u8 temp[512];  //零时空间
u8 Update_Font(void)
{
	u32 fcluster=0;	   
	u32 i;
	u32 tempsys[10];  //临时记录文件起始位置和文件大小
	u8 *ftemp=jpg_buffer;
	float prog;
	u8 t=0;		 
	FileInfoStruct FileTemp;//零时文件夹  
	tempsys[0]=0x00000000; 
	tempsys[1]=0x00000000;				    	 
	//得到根目录的簇号
	if(FAT32_Enable)fcluster=FirstDirClust;
	else fcluster=0;			   
	FileTemp=F_Search(fcluster,(unsigned char *)folder[0],T_FILE);//查找system文件夹	  
	if(FileTemp.F_StartCluster==0)return 1;	   //系统文件夹丢失	
	else						  		  
	{	 										 
		//先查找字体
		FileTemp=F_Search(FileTemp.F_StartCluster,(unsigned char *)folder[1],T_FILE);//在system文件夹下查找FONT文件夹
		if(FileTemp.F_StartCluster==0)return 2;//字体文件夹丢失	
		fcluster=FileTemp.F_StartCluster;      //字体文件夹簇号	
		    
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[3],T_SYS);//在system文件夹下查找SYS文件
		if(FileTemp.F_StartCluster==0)return 3;//UNI2GBK文件丢失	 
		LCD_DisplayStringLine(Line3,6,"Write UNI2GBK to FLASH...   %");				
		F_Open(&FileTemp);//打开该文件
		i=0; 	  
		while(F_Read(&FileTemp, ftemp))//成功读出512个字节
		{		 
			if(i<FileTemp.F_Size)//不超过文件大小
			{
				SPI_Flash_Write(ftemp,i+UNI2GBKADDR,512);//从100K字节处开始写入512个数据   
				i+=512;//增加512个字节
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(248,48,t,3,16,0x00);//显示数值
			}					    
		}
		LCD_DisplayStringLine(Line4,6,"UNICODE2GBK UPDATE SUCESS!");
		Delay_ms(500);

		tempsys[1]|=0x55000000;
		tempsys[8]=UNI2GBKADDR;			  //UNI2GBK地址
		tempsys[9]=FileTemp.F_Size;	 	  //UNI2GBK 大小

		Delay_ms(500);			    
		//printf("UNI2GBK写入FLASH完毕!\n");
		//printf("写入数据长度:%d\n",FileTemp.F_Size);
		//printf("UNI2GBKSADDR:%d\n\n",UNI2GBKADDR);
		
		//FONT16ADDR=FileTemp.F_Size+UNI2GBKADDR;//F16的首地址 
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[0],T_SYS);//在system文件夹下查找FONT16字体文件
		if(FileTemp.F_StartCluster==0)return 4;//FONT16字体文件丢失	 
	
		LCD_DisplayStringLine(Line6, 6, "Write FONT16 to FLASH...   %");		
 		//printf("开始FONT16写入FLASH...\n");		
		F_Open(&FileTemp);//打开该文件
		i=0;	  
		while(F_Read(&FileTemp, ftemp))//成功读出512个字节
		{
			if(i<FileTemp.F_Size)//不超过文件大小
			{
				SPI_Flash_Write(ftemp,i+FONT16ADDR,512);//从0开始写入512个数据   
				i+=512;//增加512个字节
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(240,96,t,3,16,0x00);//显示数值
			}
		}
		Show_Str(48, Line7, "16*16点阵字库更新完成!", 16, 0);
		Delay_ms(500);
														   
		tempsys[1]|=0x00000016;
		tempsys[2]=FONT16ADDR;
		tempsys[3]=FileTemp.F_Size;	 	  //FONT16ADDR 大小
		Delay_ms(500);
			   
		/*printf("FONT16写入FLASH完毕!\n");
		printf("写入数据长度:%d\n",FileTemp.F_Size);
		FONT12ADDR=FileTemp.F_Size+FONT16ADDR;//F16的首地址
		printf("FONT16SADDR:%d\n\n",FONT16ADDR);
		LCD_ShowString(20,60,"Write FONT12 to FLASH... ");		
		FONT12暂时不加入*/
					  
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[1],T_SYS);//在system文件夹下查找FONT12字体文件
		if(FileTemp.F_StartCluster==0)return 5;//FONT12字体文件丢失	 
		LCD_DisplayStringLine(Line9, 6, "Write FONT12 to FLASH...   %");	
		//printf("开始FONT12写入FLASH...\n");		
		F_Open(&FileTemp);//打开该文件
		i=0;	  
		while(F_Read(&FileTemp, ftemp))//成功读出512个字节
		{
			if(i<FileTemp.F_Size)//不超过文件大小
			{
				SPI_Flash_Write(ftemp,i+FONT12ADDR,512);//从0开始写入512个数据   
				i+=512;				//增加512个字节
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(240,144,t,3,16,0x00);//显示数值
			}
		}
		Show_Str(48, Line10, "12*12点阵字库更新完成!", 12, 0);
		Delay_ms(500);

		tempsys[1]|=0x00001200;	    
		tempsys[4]=FONT12ADDR;
		tempsys[5]=FileTemp.F_Size;	 	  //FONT12ADDR 大小
		Delay_ms(500);

		/*printf("FONT12写入FLASH完毕!\n");
		printf("写入数据长度:%d\n",FileTemp.F_Size);   
		printf("FONT12SADDR:%d\n\n",FONT12ADDR); */
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[2],T_SYS);//在system文件夹下查找FONT12字体文件
		if(FileTemp.F_StartCluster==0)return 6;//FONT24字体文件丢失	 
		LCD_DisplayStringLine(Line12, 6, "Write FONT24 to FLASH...   %");	
		//printf("开始FONT12写入FLASH...\n");		
		F_Open(&FileTemp);//打开该文件
		i=0;	  
		while(F_Read(&FileTemp, ftemp))//成功读出512个字节
		{
			if(i<FileTemp.F_Size)//不超过文件大小
			{
				SPI_Flash_Write(ftemp,i+FONT24ADDR,512);//从0开始写入512个数据   
				i+=512;				//增加512个字节
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(240,192,t,3,16,0x00);//显示数值
			}
		}
		Show_Str(48, Line14, "24*24点阵字库更新完成!", 24, 0);
		Delay_ms(500);

		tempsys[1]|=0x00240000;	    
		tempsys[6]=FONT24ADDR;
		tempsys[7]=FileTemp.F_Size;	 	  //FONT24ADDR 大小
		Delay_ms(500);

	}
	tempsys[0]=0x45544F46;		   //写入字库存在标志
   	SPI_Flash_Write((u8 *)tempsys,0,40);	
	Show_Str(80,Line17,"字库更新完成！",24,0);		    
	Delay_ms(3000);
	LCD_Clear(BACK_COLOR);				
	return 0;//成功
}
#endif

//用这个函数得到字体地址
//在系统使用汉字支持之前,必须调用该函数
//包括FONT16ADDR,FONT12ADDR,UNI2GBKADDR;  
u8 Font_Init(void)
{
	u32 tempsys[10];//临时记录文件起始位置和文件大小

	#ifdef SYSTEM_UPDATE
	SPI_Flash_Erase_Chip();
	#endif

	SPI_Flash_Read((u8 *)tempsys,0,40);		//读出字库标志位,看是否存在字库
	if(tempsys[0]==0x45544F46)	
	{
/*		Show_Str(80,Line10,"字库已经存在！",12,0);
		Show_Str(80,Line12,"字库已经存在！",16,0);
		Show_Str(80,Line14,"字库已经存在！",24,0);
		Delay_ms(2000);	
		LCD_Clear(BACK_COLOR); */
		return 0;	//FONTE 标志字库存在
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




























