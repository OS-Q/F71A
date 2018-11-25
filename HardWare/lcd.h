/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : lcd.h
* Author             : MCD Application Team
* Version            : V1.0.1
* Date               : 09/22/2008
* Description        : This file contains all the functions prototypes for the
*                      lcd firmware driver.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* LCD Control pins */
#define CtrlPin_RESET  GPIO_Pin_2   /* PG.14 */
#define CtrlPin_BL     GPIO_Pin_8   /* PG.15 */
#define CtrlPort_RESET    GPIOB
#define CtrlPort_BL       GPIOB

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue1          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

//频谱分段颜色
#define BOTC  0X07FF
#define MIDC  0X07FC
#define TOPC  0X069C

//MP3界面背景色
#define MP3COLOR1 BLACK
#define MP3COLOR2 0X6802
#define MP3COLOR3 0X8AA0
#define MP3COLOR4 0X500E
#define MP3COLOR5 0X380E
#define MP3COLOR6 0X000B
#define MP3COLOR7 BLACK	 

#define LRCCOLOR  GBLUE//歌词的颜色 

#define Line0          0
#define Line1          16
#define Line2          32
#define Line3          48
#define Line4          64
#define Line5          80
#define Line6          96
#define Line7          112
#define Line8          128
#define Line9          144
#define Line10		   160
#define Line11		   176
#define Line12		   192
#define Line13		   208
#define Line14		   224
#define Line15		   240
#define Line16		   256
#define Line17		   272
#define Line18		   288
#define Line19		   304
#define Line20		   320
#define Line21		   336
#define Line22		   352
#define Line23		   368
#define Line24		   384
#define Line25		   400
#define Line26		   416
#define Line27		   432
#define Line28		   448
#define Line29		   464


/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  vu16 LCD_REG;
//  u16  RESERVED[65534];
  vu16 LCD_RAM;
} LCD_TypeDef;

/* LCD is connected to the FSMC_Bank1_NOR/SRAM1 and NE1 is used as ship select signal */
#define LCD_BASE    ((u32)(0x60000000 | 0x0001FFFE))
#define LCD         ((LCD_TypeDef *) LCD_BASE)


#define LCD_W 320
#define LCD_H 480 

extern vu16 POINT_COLOR;
extern vu16 BACK_COLOR;
extern u8 DeviceCode[6];
extern u16 BGC_Buffer[16][16];
extern u16 FFT_Color[110];
extern u16 BGV_Color[1014];

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void STM3210E_LCD_Init(void);
void LCD_SetTextColor(vu16 Color);
void LCD_SetBackColor(vu16 Color);
void LCD_ClearLine(u16 Line);
void LCD_Clear(u16 Color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_DisplayChar(u16 x,u16 y,u8 ASCII,u8 size,u8 mode);
void LCD_DisplayStringLine(u16 Line, u16 Index, u8 *ptr);
u32 mypow(u8 m,u8 n);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 size,u8 mode);
void LCD_Show3Num(u16 x,u16 y,u16 num,u8 size,u8 mode);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRect(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawCircle(u16 Xpos, u16 Ypos, u16 Radius);
void LCD_FillCircle(u16 Xpos, u16 Ypos, u16 Radius);
//void LCD_DrawMonoPict(uc32 *Pict);
//void LCD_WriteBMP(u32 BmpAddress);
void Drow_Line_FFT(u16 x,u16 y,u16 color);
void Top_Delet_FFT(u8 *flyval,u32 *topstay1,u32 *topstay2);
void TFT_ShowFFT(u8* curv,u8* topv,u8 *flyv,u32 *toppos1,u32 *toppos2);
void TFT_ProBar(u16 lenth);
#define VOLBAR_X 235
#define VOLBAR_Y 245
void TFT_Volume(u16 volume);
void TFT_DrawSpeak(u16 Xpos,u16 Ypos);
u16 GradualColor(u16 scolor,u16 ecolor,u16 per);
u16 AlphaColor(u16 fcolor,u16 bcolor,u16 alpha);
void TFT_DrawCButton(u16 Xpos, u16 Ypos, u16 Radius, u16 Color1, u16 Color2, u8 mode);
void TFT_DrawCirque(u16 Xpos,u16 Ypos,u16 ORadius,u16 IRadius,u16 Color);
void TFT_DrawTriangle(u16 Xpos,u16 Ypos,u16 Height,u8 dir,u16 Color);
void TFT_CloseButton(u16 Xpos, u16 Ypos, u16 Width, u16 color1, u16 color2);
void TFT_FucButton(u16 Xpos, u16 Ypos, u16 Width, u16 color1, u16 color2, u8 *str);

/*----- Medium layer function -----*/
void LCD_DrawPoint(u16 Xpos,u16 Ypos);
void LCD_SetDisplayWindow(u16 Xsta, u16 Ysta, u16 Xend, u16 Yend);
void LCD_WriteCom(u16 LCD_Reg);
void LCD_WriteData(u16 LCD_Data);
u16 LCD_ReadData(void);
u16 LCD_ReadPoint(u16 x,u16 y);
void LCD_ReadRect(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *buff);
void LCD_WriteTempRect(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *buff);
//void LCD_WriteRAM_Prepare(void);
//void LCD_WriteRAM(u16 RGB_Code);
//void LCD_PowerOn(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void ReadDeviceCode(void);
void DMA_M2MStart(u32 SRCAddr, u32 DSTAddr, u32 num);

/*----- Low layer function -----*/
void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);

#endif /* __LCD_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
