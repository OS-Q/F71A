#ifndef __MP3PLAYER_H
#define __MP3PLAYER_H	    
#include <stm32f10x_lib.h>

void MP3_PROG(u32 pos,u32 lenth); /*
void VOL_Data_Check(u8 volx,u8 *datax);
void MP3_MsgBox_Data(u16 *sysmsg,u8 menu,u8 key);
void MP3_MsgBox_Menu(u16 *sysmsg,u8 *menu,u8 curmenu); */
u8 Mus_Play(FileInfoStruct *FileName,u16 index,u16 total);
void Mus_Viewer(void);
 
#endif

