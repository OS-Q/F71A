#include "vs1003.h"	
#include "delay.h"
#include "stdio.h"
#include "flash.h" 
#include "spec_rew.h"

//VS1003��ȫ���ܺ���
//֧��SIN���Ժ�RAM����
//��������VS1003��Ƶ����ʾ���룬����˵ʵ����զ�أ��������Լ�д��Ƶ�׷����������ǲ�����ʵ��Ƶ�ױ任��  
//����ԭ��@SCUT
//V1.1

//VS1003���ò���
//0,henh.1,hfreq.2,lenh.3,lfreq 5,������
u8 vs1003ram[6]={8,12,10,4,220,0xAA};
#define SAVE_VS_ADDR 0x006000 
//����VS1003������
//EEPROM��ַ��486~490 �����
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
//��ȡVS1003������
//EEPROM��ַ��486~490 �����
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
	 					 
//SPI1�ڶ�дһ���ֽ�
//TxData:Ҫ���͵��ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
	u8 retry=0;				 
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET)//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	  //����һ��byte 
	retry=0;
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //�����յ�������				    
} 
//����SPI1���ٶ�
//SpeedSet:1,����;0,����;
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;
	if(SpeedSet==1)//����
	{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz	
	}else//����
	{
		SPI1->CR1|=5<<3; //Fsck=Fpclk/64=1.125Mhz
	}
	SPI1->CR1|=1<<6; //SPI�豸ʹ��	  
} 
//��λVS1003
void Vs1003SoftReset(void)
{	 
	u8 retry; 				   
	while(VS_DREQ==0);//�ȴ������λ����
	SPI1_ReadWriteByte(0X00);//��������
	retry=0;
	while(Vs1003_REG_Read(SPI_MODE)!=0x0804)// �����λ,��ģʽ  
	{
		Vs1003_CMD_Write(SPI_MODE,0x0804);// �����λ,��ģʽ
		Delay_ms(2);//�ȴ�����1.35ms 
		if(retry++>100)break; 
	}	 				  
	while (VS_DREQ == 0);//�ȴ������λ����	   

	retry=0;
	while(Vs1003_REG_Read(SPI_CLOCKF)!=0X9800)//����vs1003��ʱ��,3��Ƶ ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_CLOCKF,0X9800);//����vs1003��ʱ��,3��Ƶ ,1.5xADD
		if(retry++>100)break; 
	}		   
	retry=0;
	while(Vs1003_REG_Read(SPI_AUDATA)!=0XBB81)//����vs1003��ʱ��,3��Ƶ ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_AUDATA,0XBB81);
		if(retry++>100)break; 
	}

	//Vs1003_CMD_Write(SPI_CLOCKF,0X9800); 	    
	//Vs1003_CMD_Write(SPI_AUDATA,0XBB81); //������48k��������	 
	set1003();//����VS1003����Ч				 
	ResetDecodeTime();//��λ����ʱ��	    
    //��vs1003����4���ֽ���Ч���ݣ���������SPI����
    VS_DATA_CS=0;//ѡ�����ݴ���
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	VS_DATA_CS=1;//ȡ�����ݴ���
	Delay_ms(20);
} 
//Ӳ��λMP3
void Mp3Reset(void)
{
	VS_XREST=0;
	Delay_ms(20);
	VS_DATA_CS=1;//ȡ�����ݴ���
	VS_CMD_CS=1;//ȡ�����ݴ���
	VS_XREST=1;    
	while(VS_DREQ==0);	//�ȴ�DREQΪ��
	Delay_ms(20);				 
}
//���Ҳ��� 
void VsSineTest(void)
{											    
	Mp3Reset();	 
	Vs1003_CMD_Write(0x0b,0X2020);	  //��������	 
 	Vs1003_CMD_Write(SPI_MODE,0x0820);//����vs1003�Ĳ���ģʽ	    
	while (VS_DREQ == 0);     //�ȴ�DREQΪ��
 	//��vs1003�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨vs1003�����������Ҳ���Ƶ��ֵ��������㷽����vs1003��datasheet
    VS_DATA_CS=0;//ѡ�����ݴ���
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
    //�˳����Ҳ���
    VS_DATA_CS=0;//ѡ�����ݴ���
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

    //�ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ
    VS_DATA_CS=0;//ѡ�����ݴ���      
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
    //�˳����Ҳ���
    VS_DATA_CS=0;//ѡ�����ݴ���
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
//ram ���� 																				 
void VsRamTest(void)
{
	u16 regvalue ;	   
	Mp3Reset();     
 	Vs1003_CMD_Write(SPI_MODE,0x0820);// ����vs1003�Ĳ���ģʽ
	while (VS_DREQ==0); // �ȴ�DREQΪ��
 	VS_DATA_CS=0;	       			  // xDCS = 1��ѡ��vs1003�����ݽӿ�
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
	regvalue=Vs1003_REG_Read(SPI_HDAT0); // ����õ���ֵΪ0x807F���������á�
	printf("regvalueH:%x\n",regvalue>>8);//������ 
	printf("regvalueL:%x\n",regvalue&0xff);//������ 
}     
//��VS1003д����
//address:�����ַ
//data:��������
void Vs1003_CMD_Write(u8 address,u16 data)
{  
    while(VS_DREQ==0);//�ȴ�����
	SPI1_SetSpeed(0);//���� 
	 
	VS_DATA_CS=1; //MP3_DATA_CS=1;
	VS_CMD_CS=0; //MP3_CMD_CS=0; 
	
	SPI1_ReadWriteByte(VS_WRITE_COMMAND);//����VS1003��д����
	SPI1_ReadWriteByte(address); //��ַ
	SPI1_ReadWriteByte(data>>8); //���͸߰�λ
	SPI1_ReadWriteByte(data);	 //�ڰ�λ
	VS_CMD_CS=1;          //MP3_CMD_CS=1; 
	SPI1_SetSpeed(1);//����
} 
//��VS1003д����
void Vs1003_DATA_Write(u8 data)
{
	VS_DATA_CS=0;   //MP3_DATA_CS=0;
	SPI1_ReadWriteByte(data);
	VS_DATA_CS=1;   //MP3_DATA_CS=1;
	VS_CMD_CS=1;   //MP3_CMD_CS=1; 
}
/*
//�ж�,��⵽DREQ�ŵ�һ��������.
//vs1003���Խ������ݣ�����32KB��Ƶ����
void EXTI2_IRQRoutine(void)
{ 		   			 
	u8 n;
	if(VS_DREQ)
	{
		for(n=0;n<32;n++)
		Vs1003_DATA_Write(BUFFER[n]);//������������ 
	}   		  				 
	EXTI_ClearFlag(EXTI_Line2);  //���LINE1�ϵ��жϱ�־λ 
} 
//PEN�ж�����	 
void VS1003_Int_Set(u8 en)
{
	if(en)EXTI->IMR|=1<<2;   //����line1�ϵ��ж�	  	
	else EXTI->IMR&=~(1<<2); //�ر�line1�ϵ��ж�
	EXTI_ClearFlag(EXTI_Line2);
	NVIC_ClearIRQChannelPendingBit(EXTI2_IRQChannel);	   
}
*/         
//��VS1003�ļĴ���           
//��VS1003
//ע�ⲻҪ�ñ��ٶ�ȡ,�����
u16 Vs1003_REG_Read(u8 address)
{ 
	u16 temp=0; 
    while(VS_DREQ==0);//�ǵȴ�����״̬ 
	SPI1_SetSpeed(0);//���� 
	VS_DATA_CS=1;       //MP3_DATA_CS=1;
	VS_CMD_CS=0;       //MP3_CMD_CS=0;
	SPI1_ReadWriteByte(VS_READ_COMMAND);//����VS1003�Ķ�����
	SPI1_ReadWriteByte(address);        //��ַ
	temp=SPI1_ReadWriteByte(0xff);		//��ȡ���ֽ�
	temp=temp<<8;
	temp+=SPI1_ReadWriteByte(0xff); 	//��ȡ���ֽ�
	VS_CMD_CS=1;      //MP3_CMD_CS=1; 
	SPI1_SetSpeed(1);//����
    return temp; 
}  
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665    
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//������Ԥ��ֵ
const u16 bitrate[2][16]=
{ 
{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};
//����Kbps�Ĵ�С
//�õ�mp3&wma�Ĳ�����
u16 GetHeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;            
    HEAD0=Vs1003_REG_Read(SPI_HDAT0); 
    HEAD1=Vs1003_REG_Read(SPI_HDAT1);
    switch(HEAD1)
    {        
        case 0x7665:return 0;//WAV��ʽ
        case 0X4D54:return 1;//MIDI��ʽ 
        case 0X574D://WMA��ʽ
        {
            HEAD1=HEAD0*2/25;
            if((HEAD1%10)>5)return HEAD1/10+1;
            else return HEAD1/10;
        }
        default://MP3��ʽ
        {
            HEAD1>>=3;
            HEAD1=HEAD1&0x03; 
            if(HEAD1==3)HEAD1=1;
            else HEAD1=0;
            return bitrate[HEAD1][HEAD0>>12];
        }
    } 
}  
//�������ʱ��                          
void ResetDecodeTime(void)
{
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);//��������
}
//�õ�mp3�Ĳ���ʱ��n sec
u16 GetDecodeTime(void)
{ 
    return Vs1003_REG_Read(SPI_DECODE_TIME);   
} 
//����Ƶ�׷����Ĵ��뵽VS1003
void LoadPatch(void)
{
	u16 i;
	for (i=0;i<943;i++)Vs1003_CMD_Write(atab[i],dtab[i]); 
	Delay_ms(10);
}
//�õ�Ƶ������
void GetSpec(u8 *p)
{
	u8 byteIndex=0;
	u8 temp;
	Vs1003_CMD_Write(SPI_WRAMADDR,0x1804);                                                                                             
	for (byteIndex=0;byteIndex<14;byteIndex++) 
	{                                                                               
		temp=Vs1003_REG_Read(SPI_WRAM)&0x00FF;//ȡС��100����   
		*p++=temp/2;
	} 
}
/*
void SPI1_RST(void)
{
	RCC->APB2RSTR|=1<<12;//��λSPI1
	delay_ms(10); 	
	RCC->APB2RSTR&=~(1<<12);//������λSPI1
	delay_ms(10); 
	SPI1->CR1|=0<<10;//ȫ˫��ģʽ	
	SPI1->CR1|=1<<9; //���nss����
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; //SPI����
	SPI1->CR1|=0<<11;//8bit���ݸ�ʽ	
	SPI1->CR1|=1<<1; //����ģʽ��SCKΪ1 CPOL=1
	SPI1->CR1|=1<<0; //���ݲ����ӵڶ���ʱ����ؿ�ʼ,CPHA=1  
	SPI1->CR1|=6<<3; //Fsck=Fpclk/128 =562.5khz
	SPI1->CR1|=0<<7; //MSBfirst 
	
	SPI1->CR1|=1<<6; //SPI�豸ʹ��
}
*/	  
//�趨vs1003���ŵ������͸ߵ��� 
void set1003(void)
{
    u8 t;
    u16 bass=0; //�ݴ������Ĵ���ֵ
    u16 volt=0; //�ݴ�����ֵ
    u8 vset=0;  //�ݴ�����ֵ 	 
    vset=255-vs1003ram[4];//ȡ��һ��,�õ����ֵ,��ʾ���ı�ʾ 
    volt=vset;
    volt<<=8;
    volt+=vset;//�õ��������ú��С
     //0,henh.1,hfreq.2,lenh.3,lfreq        
    for(t=0;t<4;t++)
    {
        bass<<=4;
        bass+=vs1003ram[t]; 
    }     
	Vs1003_CMD_Write(SPI_BASS,bass);//BASS   
    Vs1003_CMD_Write(SPI_VOL,volt); //������ 
}

void SetVolume(u16 volume)
{
    u8 vset;  //�ݴ�����ֵ 
	if(volume > 25) volume = 25;
	if(volume<=5) volume *= 18;
	else if(volume<=10) volume = 90+(volume-5)*14;
	else if(volume<=15) volume = 160+(volume-10)*9;
	else if(volume<=20) volume = 205+(volume-15)*5;
	else volume = 230+(volume-20)*4;
	vs1003ram[4] = volume;
	volume=255-volume;//ȡ��һ��,�õ����ֵ,��ʾ���ı�ʾ 
    vset = volume;
	volume <<= 8;
    volume += vset;	//�õ��������ú��С
	Vs1003_CMD_Write(SPI_VOL,volume); //������
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
    

//��ʼ��VS1003��IO��	 
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
    //EXTI_Mode������EXTI_Mode_Interrupt��EXTI_Mode_Event
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //EXTI_Trigger������EXTI_Trigger_Rising��EXTI_Trigger_Falling
    //��EXTI_Trigger_Rising_Falling
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);			 
    //��ʼ��EXTI2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;
	//��ռ���ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	//�����ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	//�ж�ͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//��ʼ��EXTI2
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


