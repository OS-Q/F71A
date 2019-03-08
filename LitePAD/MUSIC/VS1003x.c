#include "vs1003.h"	
#include "delay.h"
#include "stdio.h"
#include "flash.h" 
#include "spec_rew.h"

//VS1003的全功能函数
//支持SIN测试和RAM测试
//并加入了VS1003的频谱显示代码，不过说实话不咋地，还不如自己写的频谱分析，怀疑是不是真实的频谱变换？  
//正点原子@SCUT
//V1.1

//VS1003设置参数
//0,henh.1,hfreq.2,lenh.3,lfreq 5,主音量
u8 vs1003ram[6]={8,12,10,4,220,0xAA};
#define SAVE_VS_ADDR 0x006000 
//保存VS1003的设置
//EEPROM地址：486~490 共五个
void Save_VS_Set(void)
{
	s16 n;
	u8 temp;
	for(n=0;n<=400;n++)
	{
		if(n==100)
		{
			SPI_Flash_Erase_Sector(SAVE_VS_ADDR>>12);
			n=-1;
			continue;
		}
		SPI_Flash_Read(&temp, SAVE_VS_ADDR+n*6+5, 1);
		if(temp==0xFF)
		{
			SPI_Flash_Write(vs1003ram, SAVE_VS_ADDR+n*6, 6);
			break;
		}
		else continue;
	}
		 
}
//读取VS1003的设置
//EEPROM地址：486~490 共五个
void Read_VS_Set(void)
{
	u16 n;
	u8 temp;
	for(n=0;n<400;n++)
	{
		SPI_Flash_Read(&temp, SAVE_VS_ADDR+n*6+5, 1);
		if(temp==0xAA)
		{
			SPI_Flash_Read(&temp, SAVE_VS_ADDR+n*6+11, 1);
			if(temp==0xFF)
			{
				SPI_Flash_Read(vs1003ram, SAVE_VS_ADDR+n*6, 6);
				break;
			}
			else continue;
		}
	}
}
	 					 
//SPI1口读写一个字节
//TxData:要发送的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
	u8 retry=0;				 
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //返回收到的数据				    
} 
//设置SPI1的速度
//SpeedSet:1,高速;0,低速;
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;
	if(SpeedSet==1)//高速
	{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz	
	}else//低速
	{
		SPI1->CR1|=5<<3; //Fsck=Fpclk/64=1.125Mhz
	}
	SPI1->CR1|=1<<6; //SPI设备使能	  
} 
//软复位VS1003
void Vs1003SoftReset(void)
{	 
	u8 retry; 				   
	while(VS_DREQ==0);//等待软件复位结束
	SPI1_ReadWriteByte(0X00);//启动传输
	retry=0;
	while(Vs1003_REG_Read(SPI_MODE)!=0x0804)// 软件复位,新模式  
	{
		Vs1003_CMD_Write(SPI_MODE,0x0804);// 软件复位,新模式
		Delay_ms(2);//等待至少1.35ms 
		if(retry++>100)break; 
	}	 				  
	while (VS_DREQ == 0);//等待软件复位结束	   

	retry=0;
	while(Vs1003_REG_Read(SPI_CLOCKF)!=0X9800)//设置vs1003的时钟,3倍频 ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_CLOCKF,0X9800);//设置vs1003的时钟,3倍频 ,1.5xADD
		if(retry++>100)break; 
	}		   
	retry=0;
	while(Vs1003_REG_Read(SPI_AUDATA)!=0XBB81)//设置vs1003的时钟,3倍频 ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_AUDATA,0XBB81);
		if(retry++>100)break; 
	}

	//Vs1003_CMD_Write(SPI_CLOCKF,0X9800); 	    
	//Vs1003_CMD_Write(SPI_AUDATA,0XBB81); //采样率48k，立体声	 
	set1003();//设置VS1003的音效				 
	ResetDecodeTime();//复位解码时间	    
    //向vs1003发送4个字节无效数据，用以启动SPI发送
    VS_DATA_CS=0;//选中数据传输
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	VS_DATA_CS=1;//取消数据传输
	Delay_ms(20);
} 
//硬复位MP3
void Mp3Reset(void)
{
	VS_XREST=0;
	Delay_ms(20);
	VS_DATA_CS=1;//取消数据传输
	VS_CMD_CS=1;//取消数据传输
	VS_XREST=1;    
	while(VS_DREQ==0);	//等待DREQ为高
	Delay_ms(20);				 
}
//正弦测试 
void VsSineTest(void)
{											    
	Mp3Reset();	 
	Vs1003_CMD_Write(0x0b,0X2020);	  //设置音量	 
 	Vs1003_CMD_Write(SPI_MODE,0x0820);//进入vs1003的测试模式	    
	while (VS_DREQ == 0);     //等待DREQ为高
 	//向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n = 0x24, 设定vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
    VS_DATA_CS=0;//选中数据传输
	SPI1_ReadWriteByte(0x53);
	SPI1_ReadWriteByte(0xef);
	SPI1_ReadWriteByte(0x6e);
	SPI1_ReadWriteByte(0x24);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Delay_ms(100);
	VS_DATA_CS=1; 
    //退出正弦测试
    VS_DATA_CS=0;//选中数据传输
	SPI1_ReadWriteByte(0x45);
	SPI1_ReadWriteByte(0x78);
	SPI1_ReadWriteByte(0x69);
	SPI1_ReadWriteByte(0x74);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Delay_ms(100);
	VS_DATA_CS=1;		 

    //再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值
    VS_DATA_CS=0;//选中数据传输      
	SPI1_ReadWriteByte(0x53);
	SPI1_ReadWriteByte(0xef);
	SPI1_ReadWriteByte(0x6e);
	SPI1_ReadWriteByte(0x44);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Delay_ms(100);
	VS_DATA_CS=1;
    //退出正弦测试
    VS_DATA_CS=0;//选中数据传输
	SPI1_ReadWriteByte(0x45);
	SPI1_ReadWriteByte(0x78);
	SPI1_ReadWriteByte(0x69);
	SPI1_ReadWriteByte(0x74);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Delay_ms(100);
	VS_DATA_CS=1;	 
}	 
//ram 测试 																				 
void VsRamTest(void)
{
	u16 regvalue ;	   
	Mp3Reset();     
 	Vs1003_CMD_Write(SPI_MODE,0x0820);// 进入vs1003的测试模式
	while (VS_DREQ==0); // 等待DREQ为高
 	VS_DATA_CS=0;	       			  // xDCS = 1，选择vs1003的数据接口
	SPI1_ReadWriteByte(0x4d);
	SPI1_ReadWriteByte(0xea);
	SPI1_ReadWriteByte(0x6d);
	SPI1_ReadWriteByte(0x54);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Delay_ms(50);  
	VS_DATA_CS=1;
	regvalue=Vs1003_REG_Read(SPI_HDAT0); // 如果得到的值为0x807F，则表明完好。
	printf("regvalueH:%x\n",regvalue>>8);//输出结果 
	printf("regvalueL:%x\n",regvalue&0xff);//输出结果 
}     
//向VS1003写命令
//address:命令地址
//data:命令数据
void Vs1003_CMD_Write(u8 address,u16 data)
{  
    while(VS_DREQ==0);//等待空闲
	SPI1_SetSpeed(0);//低速 
	 
	VS_DATA_CS=1; //MP3_DATA_CS=1;
	VS_CMD_CS=0; //MP3_CMD_CS=0; 
	
	SPI1_ReadWriteByte(VS_WRITE_COMMAND);//发送VS1003的写命令
	SPI1_ReadWriteByte(address); //地址
	SPI1_ReadWriteByte(data>>8); //发送高八位
	SPI1_ReadWriteByte(data);	 //第八位
	VS_CMD_CS=1;          //MP3_CMD_CS=1; 
	SPI1_SetSpeed(1);//高速
} 
//向VS1003写数据
void Vs1003_DATA_Write(u8 data)
{
	VS_DATA_CS=0;   //MP3_DATA_CS=0;
	SPI1_ReadWriteByte(data);
	VS_DATA_CS=1;   //MP3_DATA_CS=1;
	VS_CMD_CS=1;   //MP3_CMD_CS=1; 
}
/*
//中断,检测到DREQ脚的一个上升沿.
//vs1003可以接收数据，发送32KB音频数据
void EXTI2_IRQRoutine(void)
{ 		   			 
	u8 n;
	if(VS_DREQ)
	{
		for(n=0;n<32;n++)
		Vs1003_DATA_Write(BUFFER[n]);//发送音乐数据 
	}   		  				 
	EXTI_ClearFlag(EXTI_Line2);  //清除LINE1上的中断标志位 
} 
//PEN中断设置	 
void VS1003_Int_Set(u8 en)
{
	if(en)EXTI->IMR|=1<<2;   //开启line1上的中断	  	
	else EXTI->IMR&=~(1<<2); //关闭line1上的中断
	EXTI_ClearFlag(EXTI_Line2);
	NVIC_ClearIRQChannelPendingBit(EXTI2_IRQChannel);	   
}
*/         
//读VS1003的寄存器           
//读VS1003
//注意不要用倍速读取,会出错
u16 Vs1003_REG_Read(u8 address)
{ 
	u16 temp=0; 
    while(VS_DREQ==0);//非等待空闲状态 
	SPI1_SetSpeed(0);//低速 
	VS_DATA_CS=1;       //MP3_DATA_CS=1;
	VS_CMD_CS=0;       //MP3_CMD_CS=0;
	SPI1_ReadWriteByte(VS_READ_COMMAND);//发送VS1003的读命令
	SPI1_ReadWriteByte(address);        //地址
	temp=SPI1_ReadWriteByte(0xff);		//读取高字节
	temp=temp<<8;
	temp+=SPI1_ReadWriteByte(0xff); 	//读取低字节
	VS_CMD_CS=1;      //MP3_CMD_CS=1; 
	SPI1_SetSpeed(1);//高速
    return temp; 
}  
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665    
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//比特率预定值
const u16 bitrate[2][16]=
{ 
{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};
//返回Kbps的大小
//得到mp3&wma的波特率
u16 GetHeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;            
    HEAD0=Vs1003_REG_Read(SPI_HDAT0); 
    HEAD1=Vs1003_REG_Read(SPI_HDAT1);
    switch(HEAD1)
    {        
        case 0x7665:return 0;//WAV格式
        case 0X4D54:return 1;//MIDI格式 
        case 0X574D://WMA格式
        {
            HEAD1=HEAD0*2/25;
            if((HEAD1%10)>5)return HEAD1/10+1;
            else return HEAD1/10;
        }
        default://MP3格式
        {
            HEAD1>>=3;
            HEAD1=HEAD1&0x03; 
            if(HEAD1==3)HEAD1=1;
            else HEAD1=0;
            return bitrate[HEAD1][HEAD0>>12];
        }
    } 
}  
//重设解码时间                          
void ResetDecodeTime(void)
{
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);//操作两次
}
//得到mp3的播放时间n sec
u16 GetDecodeTime(void)
{ 
    return Vs1003_REG_Read(SPI_DECODE_TIME);   
} 
//加载频谱分析的代码到VS1003
void LoadPatch(void)
{
	u16 i;
	for (i=0;i<943;i++)Vs1003_CMD_Write(atab[i],dtab[i]); 
	Delay_ms(10);
}
//得到频谱数据
void GetSpec(u8 *p)
{
	u8 byteIndex=0;
	u8 temp;
	Vs1003_CMD_Write(SPI_WRAMADDR,0x1804);                                                                                             
	for (byteIndex=0;byteIndex<14;byteIndex++) 
	{                                                                               
		temp=Vs1003_REG_Read(SPI_WRAM)&0x00FF;//取小于100的数   
		*p++=temp/2;
	} 
}
/*
void SPI1_RST(void)
{
	RCC->APB2RSTR|=1<<12;//复位SPI1
	delay_ms(10); 	
	RCC->APB2RSTR&=~(1<<12);//结束复位SPI1
	delay_ms(10); 
	SPI1->CR1|=0<<10;//全双工模式	
	SPI1->CR1|=1<<9; //软件nss管理
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; //SPI主机
	SPI1->CR1|=0<<11;//8bit数据格式	
	SPI1->CR1|=1<<1; //空闲模式下SCK为1 CPOL=1
	SPI1->CR1|=1<<0; //数据采样从第二个时间边沿开始,CPHA=1  
	SPI1->CR1|=6<<3; //Fsck=Fpclk/128 =562.5khz
	SPI1->CR1|=0<<7; //MSBfirst 
	
	SPI1->CR1|=1<<6; //SPI设备使能
}
*/	  
//设定vs1003播放的音量和高低音 
void set1003(void)
{
    u8 t;
    u16 bass=0; //暂存音调寄存器值
    u16 volt=0; //暂存音量值
    u8 vset=0;  //暂存音量值 	 
    vset=255-vs1003ram[4];//取反一下,得到最大值,表示最大的表示 
    volt=vset;
    volt<<=8;
    volt+=vset;//得到音量设置后大小
     //0,henh.1,hfreq.2,lenh.3,lfreq        
    for(t=0;t<4;t++)
    {
        bass<<=4;
        bass+=vs1003ram[t]; 
    }     
	Vs1003_CMD_Write(SPI_BASS,bass);//BASS   
    Vs1003_CMD_Write(SPI_VOL,volt); //设音量 
}

void SetVolume(u16 volume)
{
    u8 vset;  //暂存音量值 
	if(volume > 25) volume = 25;
	if(volume<=5) volume *= 18;
	else if(volume<=10) volume = 90+(volume-5)*14;
	else if(volume<=15) volume = 160+(volume-10)*9;
	else if(volume<=20) volume = 205+(volume-15)*5;
	else volume = 230+(volume-20)*4;
	vs1003ram[4] = volume;
	volume=255-volume;//取反一下,得到最大值,表示最大的表示 
    vset = volume;
	volume <<= 8;
    volume += vset;	//得到音量设置后大小
	Vs1003_CMD_Write(SPI_VOL,volume); //设音量
}

u16 GetVolume(void)
{
    u16 volume;
	volume=vs1003ram[4]; 

	if(volume<=90) volume /= 18;
	else if(volume<=160) volume = (volume-90)/14 + 5;
	else if(volume<=205) volume = (volume-160)/9 + 10;
	else if(volume<=230) volume = (volume-205)/5 + 15;
	else volume = (volume-230)/4 + 20;
	return volume;
}
    

//初始化VS1003的IO口	 
void Vs1003_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
	
	/* Enable SPI1 GPIOA clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
						 
	/* Set PA.05(XSCLK), PA.07(XSI) as alternate function push pull */     
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;	                                                              
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	VS_CMD_CS=1;
	VS_DATA_CS=1;
	VS_XREST=1;

/*	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);                         
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);

    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    //EXTI_Mode参数：EXTI_Mode_Interrupt、EXTI_Mode_Event
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //EXTI_Trigger参数：EXTI_Trigger_Rising、EXTI_Trigger_Falling
    //、EXTI_Trigger_Rising_Falling
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);			 
    //初始化EXTI2中断通道
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;
	//抢占优先级 1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	//子优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	//中断通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化EXTI2
	NVIC_Init(&NVIC_InitStructure);*/	
	
	/* Initialize the SPI_Direction member */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	
	/* initialize the SPI_Mode member */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	
	/* initialize the SPI_DataSize member */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	
	/* Initialize the SPI_CPOL member */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	
	/* Initialize the SPI_CPHA member */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	
	/* Initialize the SPI_NSS member */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	
	/* Initialize the SPI_BaudRatePrescaler member */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	
	/* Initialize the SPI_FirstBit member */
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	
	/* Initialize the SPI_CRCPolynomial member */
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);	
}


