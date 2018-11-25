#ifndef __GUI_H
#define __GUI_H
#include <stm32f10x_lib.h>
	    
//Mini STM32开发板
//RTC实时时钟 驱动代码			 
//正点原子@ALIENTEK
//2010/6/6	
#define MOVE_LEFT   0X10
#define MOVE_RIGHT  0X20
#define MOVE_UP     0X40
#define MOVE_DOWN   0X80
	
#define	KEY_NEXT 0X01
#define KEY_PREV 0X02
#define KEY_FUNC 0X03
///////////////////////////////////////
#define FRAME1 0XBC40 //棕色
#define FRAME2 0XDED7 //窗体色	 
//功能选择背景色
#define Bro_TOP 0X038A
#define Bro_MID 0XEFC0
#define Bro_BTM 0X81BF
extern u8 BrowseAll;
void GUI_DrawLine(u16 x,u16 y,u16 xlen,u16 ylen,u16 color);
void GUI_Draw_Button(u16 x,u16 y,u16 xlen,u16 ylen,u8 *str,u16 color);
void Load_Draw_Dialog(void);
void Load_Game_ICO(u8 size);
u8 Load_Fun_Sel(const u8 * *list,const u8 *title,u8 *func,u8 len,u8 sel);
void Load_Sys_ICO(void);
void Select_Menu(u8 M_Type,u8 Cnt,u8 En);
u8 Touch_To_Num(u8 M_Type);
u8 KEY_Move_Check(void);//触点滑动检测
u16 File_Browse(u32 file_type);
void Load_Gui_Bro(u8 * str);
void Load_Gui_Show(u8 *lbtstr,u8 *mbtstr,u8 *rbtstr);
void Load_Gui_Mp3(u16 funmark);
void Load_Gui_Set_EQ(u8 kind);

void LCD_DrawDLine(u16 x1, u16 y1, u16 x2, u16 y2);  //画2个单位宽的线

void Draw_Color_Table(u16 x,u16 y);//画颜色表

#endif


