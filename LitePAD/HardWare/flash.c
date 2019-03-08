#include "flash.h" 
#include "delay.h"
#include "jpegbmp.h" 
#include "lcd.h" 
//Mini STM32开发板
//W25X16 驱动函数 
//正点原子@ALIENTEK
//2010/6/13
//V1.0

//4Kbytes为一个Sector
//16个扇区为1个Block
//ENF16
//容量为2M字节,共有32个Block,512个Sector 

//初始化SPI FLASH的IO口
////返回flash状态
u16 SPI_Flash_Init(void)
{
	u16 temp;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

    //使能GPIOB, SPI2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
 
    //PB.13(SPI2_CLK), PB.15(SPI2_MOSI)复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	//PB.12(SPI2_CS)通用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	//PB.14(SPI2_MISO)上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Initialize the SPI_Direction member */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	
	/* initialize the SPI_Mode member */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	
	/* initialize the SPI_DataSize member */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	
	/* Initialize the SPI_CPOL member */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	
	/* Initialize the SPI_CPHA member */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	
	/* Initialize the SPI_NSS member */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	
	/* Initialize the SPI_BaudRatePrescaler member */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	
	/* Initialize the SPI_FirstBit member */
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	
	/* Initialize the SPI_CRCPolynomial member */
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);

	temp=SPI_Flash_ReadSR();
	temp=SPI_Flash_WAKEUP();
	SPI_FLASH_Write_Disable();
	temp=SPI_Flash_ReadID();	
	SPI_FLASH_Write_SR(0x00);	
	temp=SPI_Flash_ReadSR();
	return temp;
}

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	while((SPI2->SR & SPI_I2S_FLAG_TXE)==RESET)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI2->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR & SPI_I2S_FLAG_RXNE)==RESET) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI2->DR;          //返回收到的数据				    
}
  

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  RV BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_CS=0;                            //使能器件   
	SPI2_ReadWriteByte(EN25F16_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPI2_ReadWriteByte(EN25F16_NOP);             //读取一个字节  
	SPI_FLASH_CS=1;                            //取消片选     
	return byte;   
} 
//写SPI_FLASH状态寄存器
//只有SPR,BP2,BP1,BP0(bit 7,4,3,2)可以写!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_Flash_Wait_Busy();
	SPI_FLASH_Write_Enable();
	SPI_FLASH_CS=0;                       //使能器件   
	SPI2_ReadWriteByte(EN25F16_WriteStatusReg);   //发送写取状态寄存器命令    
	SPI2_ReadWriteByte(sr);               //写入一个字节  
	SPI_FLASH_CS=1;                        //取消片选
	Delay_ms(10);
	SPI_Flash_Wait_Busy();     	      
}   
//SPI_FLASH写使能	
//将WEL置位   
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_WriteEnable);      //发送写使能  
	SPI_FLASH_CS=1;                            //取消片选     	      
} 
//SPI_FLASH写禁止	
//将WEL清零  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_WriteDisable);     //发送写禁止指令    
	SPI_FLASH_CS=1;                            //取消片选     	      
} 			    
//读取芯片ID EN25F16的ID:0X1C14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS=0;				    
	SPI2_ReadWriteByte(EN25F16_ManufactDeviceID);//发送读取ID命令	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(EN25F16_NOP)<<8;  
	Temp|=SPI2_ReadWriteByte(EN25F16_NOP);	 
	SPI_FLASH_CS=1;				    
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_ReadData);         //发送读取命令   
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI2_ReadWriteByte(EN25F16_NOP);   //循环读数  
    }
	SPI_FLASH_CS=1;                            //取消片选     	      
}  
#ifdef FLASH_WRITE_EN
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;
    SPI_Flash_Wait_Busy();  
	SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_PageProgram);      //发送写页命令   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)
	SPI2_ReadWriteByte(pBuffer[i]);        //循环写数  
	SPI_FLASH_CS=1;                        //取消片选 
	Delay_us(1000);
	SPI_Flash_Wait_Busy();				   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)  		   
//u8 SPI_FLASH_BUF[4096];

//将jpeg解码的缓存用作SPI FLASH的写缓存!不能有冲突!
//在内存充足的情况下建议使用独立的缓存空间

void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 *SPI_FLASH_BUF=(u8*)iclip;
	secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			SPI_Flash_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	}	 	 
}
#endif
//擦除整个芯片
//整片擦除时间:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//等待时间超长...
void SPI_Flash_Erase_Chip(void)   
{                                             
    u8 time=0;
	SPI_Flash_Wait_Busy();
	SPI_FLASH_Write_Enable();                  //SET WEL    
  	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_ChipErase);        //发送片擦除命令  
	SPI_FLASH_CS=1;                            //取消片选 
	LCD_DisplayStringLine(Line10,6,"EN25F16 is erasing full chip...");
	LCD_DisplayStringLine(Line12,12,"Past Time:  s");
	while ((SPI_Flash_ReadSR()&0x01)==0x01)	   //等待芯片擦除结束
	{
		Delay_ms(1000);
		time++;
    	LCD_ShowNum(176,192,time,2,16,0x00);
	}
	LCD_ClearLine(Line10);		      
   	LCD_ClearLine(Line12);			  
}  

//擦除一个块
//Dst_Addr:块地址 0~31 for en25f16
//擦除一个山区的最少时间:150ms
void SPI_Flash_Erase_Block(u32 Dst_Addr)   
{   
	Dst_Addr*=65536;
    SPI_Flash_Wait_Busy(); 
	SPI_FLASH_Write_Enable();                  //SET WEL 	   
  	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_BlockErase);      //发送扇区擦除指令 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1; 						   //取消片选  
	Delay_ms(800);                              	      
    SPI_Flash_Wait_Busy();   				   //等待擦除完成
}  
 
//擦除一个扇区
//Dst_Addr:扇区地址 0~511 for w25x16
//擦除一个山区的最少时间:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
    SPI_Flash_Wait_Busy();
	SPI_FLASH_Write_Enable();                  //SET WEL 	    
  	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_SectorErase);      //发送扇区擦除指令 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1; 				 //取消片选 
	Delay_ms(150);                              	      
    SPI_Flash_Wait_Busy();   	      //等待擦除完成
}  
//等待空闲
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_PowerDown);        //发送掉电命令  
	SPI_FLASH_CS=1;                            //取消片选     	      
    Delay_us(3);                               //等待TPD  
}   
//唤醒
u8 SPI_Flash_WAKEUP(void)   
{  
  	u8 id;
	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(EN25F16_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	id=SPI2_ReadWriteByte(EN25F16_NOP);
	SPI_FLASH_CS=1;                            //取消片选     	      
    Delay_us(3);
	return id;                               //等待TRES1
} 


