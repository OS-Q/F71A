#include "temp.h"
#include "delay.h"


void Temp_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_DeInit(ADC1);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//单次转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部事件软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//规则通道转换序列长度为1，只转换序列1
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//使能外部事件触发转换 
	//配置规则通道转换序列1为ADC_Channel_16，即温度传感器，采样时间为71.5周期
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
	temperate=(float)temp*(3.2/4096);//得到温度传感器的电压值
	temperate=(1.43-temperate)/0.0043+25;//计算出当前温度值	 
	temperate*=10;//扩大十倍,使用小数点后一位
	return (int)temperate;
}

void Temp_Close(void)
{
	ADC_Cmd(ADC1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
}

