#ifndef _SYSFILE_H_
#define _SYSFILE_H_		 	 
#include <stm32f10x_lib.h>
 
//Mini STM32开发板
//系统文件查找代码					  
//正点原子@ALIENTEK
//2010/6/18
	 
extern u32 PICCLUSTER;//图片文件夹首地址
extern u32 MUSCLUSTER;//音乐文件夹地址
extern u32 TXTCLUSTER;//电子书文件夹地址
extern u32 LOGOCLUSTER;//logo图片地址
extern u32 sys_ico[13];  //系统图标缓存区!不能篡改!
extern u32 file_ico[5]; //文件图标缓存区 folder;mus;pic;book;
extern u32 *Pic_Addr;	

u8 SysInfoGet(u8 sel);//获取系统文件信息
void Get_Game_Pic_Str(u8 t,u8 *str);
u8 LoadLOGO(void);
#endif




