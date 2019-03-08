#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include <stm32f10x_lib.h>	 
//Mini STM32������
//���ĺ���֧�ֳ��� V1.1					   
//����ԭ��@ALIENTEK
//2010/5/23	

#define EN_UPDATE_FONT //ʹ���������,ͨ���ر�����ʵ�ֽ�ֹ�ֿ����
//#define SYSTEM_UPDATE   //����ȫ��flash���ݣ���ִ��ȫƬ����

//flash�洢�ռ���䣬���������֣�EN25F16����2MB�ռ�
//ÿ����4KB������512��������������0-511
//0-9����������ϵͳʹ��
//0�����洢�ֿ������Ϣ
//1�����洢����У׼��Ϣ 
#define FONT16ADDR   0x00A000	   //10-73�����洢16*16����GB2312�ֿ�
#define FONT12ADDR   0x04A000	   //74-109�����洢12*12����GB2312�ֿ�
#define FONT24ADDR   0x06E000	   //110-253�����洢24*124����GB2312�ֿ�
#define UNI2GBKADDR	 0x0FE000	   //254-264�����洢unicodeתGBK��
#define GBK2UNIADDR  0x109000	   //265-276�����洢GBKתunicode��0x115000��

u8 Update_Font(void);//�����ֿ�   
u8 Font_Init(void);//��ʼ���ֿ�
//void DMA_M2MInit(void);
//void DMA_M2MStart(u32 SRCAddr, u32 DSTAddr, u32 num);
#endif





















