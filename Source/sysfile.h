#ifndef _SYSFILE_H_
#define _SYSFILE_H_		 	 
#include <stm32f10x_lib.h>
 
//Mini STM32������
//ϵͳ�ļ����Ҵ���					  
//����ԭ��@ALIENTEK
//2010/6/18
	 
extern u32 PICCLUSTER;//ͼƬ�ļ����׵�ַ
extern u32 MUSCLUSTER;//�����ļ��е�ַ
extern u32 TXTCLUSTER;//�������ļ��е�ַ
extern u32 LOGOCLUSTER;//logoͼƬ��ַ
extern u32 sys_ico[13];  //ϵͳͼ�껺����!���ܴ۸�!
extern u32 file_ico[5]; //�ļ�ͼ�껺���� folder;mus;pic;book;
extern u32 *Pic_Addr;	

u8 SysInfoGet(u8 sel);//��ȡϵͳ�ļ���Ϣ
void Get_Game_Pic_Str(u8 t,u8 *str);
u8 LoadLOGO(void);
#endif




