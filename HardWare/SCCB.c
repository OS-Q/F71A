#include "sys.h"
#include "delay.h"
#include "SCCB.h"
/*
-----------------------------------------------
   ����: ��ʼ��ģ��SCCB�ӿ�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void InitSCCB(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    //ʹ��GPIOGʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//SCCB_SCKͨ�ÿ�©���
    GPIO_InitStructure.GPIO_Pin = SCCB_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(SCCB_SCK_PORT, &GPIO_InitStructure);
	//SCCB_SDAͨ�ÿ�©���
    GPIO_InitStructure.GPIO_Pin = SCCB_SDA_PIN;
    GPIO_Init(SCCB_SDA_PORT, &GPIO_InitStructure);
	SIO_C_SET;
	SIO_D_SET;	
}
/*
-----------------------------------------------
   ����: start����,SCCB����ʼ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void startSCCB(void)
{
	SIO_D_SET;     //�����߸ߵ�ƽ
    Delay_us(10);

    SIO_C_SET;	   //��ʱ���߸ߵ�ʱ���������ɸ�����
    Delay_us(10);
 
    SIO_D_CLR;
    Delay_us(10);

    SIO_C_CLR;	 //�����߻ָ��͵�ƽ��������������Ҫ
    Delay_us(10);


}
/*
-----------------------------------------------
   ����: stop����,SCCB��ֹͣ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void stopSCCB(void)
{
	SIO_D_CLR;
    Delay_us(10);
 
    SIO_C_SET;
    Delay_us(10);
  
    SIO_D_SET;
    Delay_us(10);
}

/*
-----------------------------------------------
   ����: noAck,����������ȡ�е����һ����������
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void noAck(void)
{
	
	SIO_D_SET;
	Delay_us(10);
	
	SIO_C_SET;
	Delay_us(10);
	
	SIO_C_CLR;
	Delay_us(10);
	
	SIO_D_CLR;
	Delay_us(10);
}

/*
-----------------------------------------------
   ����: д��һ���ֽڵ����ݵ�SCCB
   ����: д������
 ����ֵ: ���ͳɹ�����1������ʧ�ܷ���0
-----------------------------------------------
*/
u8 SCCBwriteByte(u8 m_data)
{
	u8 j,tem;

	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if((m_data<<j)&0x80)
		{
			SIO_D_SET;
		}
		else
		{
			SIO_D_CLR;
		}
		Delay_us(10);
		SIO_C_SET;
		Delay_us(10);
		SIO_C_CLR;
		Delay_us(10);

	}
	Delay_us(10);
	
	SIO_D_IN;/*����SDAΪ����*/
	Delay_us(50);
	SIO_C_SET;
	Delay_us(50);
	if(SIO_D_STATE)
	{
		tem=0;   //SDA=1����ʧ�ܣ�����0
	}
	else
	{
		tem=1;   //SDA=0���ͳɹ�������1
	}
	SIO_C_CLR;
	Delay_us(10);	
    SIO_D_OUT;/*����SDAΪ���*/

	return(tem);  
}

/*
-----------------------------------------------
   ����: һ���ֽ����ݶ�ȡ���ҷ���
   ����: ��
 ����ֵ: ��ȡ��������
-----------------------------------------------
*/
u8 SCCBreadByte(void)
{
	u8 read,j;
	read=0x00;
	
	SIO_D_IN;/*����SDAΪ����*/
	Delay_us(10);
	for(j=8;j>0;j--) //ѭ��8�ν�������
	{		     
		Delay_us(10);
		SIO_C_SET;
		Delay_us(10);
		read=read<<1;
		if(SIO_D_STATE) 
		{
			read=read+1;
		}
		SIO_C_CLR;
		Delay_us(10);
	}
    SIO_D_OUT;/*����SDAΪ���*/	
	return(read);
	
}
