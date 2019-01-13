#include "temp.h"
#include "delay.h"


void Temp_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_DeInit(ADC1);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�ⲿ�¼��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;//����ͨ��ת�����г���Ϊ1��ֻת������1
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//ʹ���ⲿ�¼�����ת�� 
	//���ù���ͨ��ת������1ΪADC_Channel_16�����¶ȴ�����������ʱ��Ϊ71.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_71Cycles5); 
	ADC_Cmd(ADC1, ENABLE);
	Delay_us(2);	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));                     
}

int Get_Temperature(void)
{
	float temperate;
	u16 temp;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	Delay_us(5);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET);
	temp=ADC_GetConversionValue(ADC1);   
	temperate=(float)temp*(3.2/4096);//�õ��¶ȴ������ĵ�ѹֵ
	temperate=(1.43-temperate)/0.0043+25;//�������ǰ�¶�ֵ	 
	temperate*=10;//����ʮ��,ʹ��С�����һλ
	return (int)temperate;
}

void Temp_Close(void)
{
	ADC_Cmd(ADC1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
}

