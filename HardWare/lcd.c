/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : lcd.c
* Author             : MCD Application Team
* Version            : V1.0.1
* Date               : 09/22/2008
* Description        : This file includes the LCD driver for AM-240320L8TNQW00H 
*                     (LCD_ILI9320) Liquid Crystal Display Module of STM3210E-EVAL
*                      board.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "fonts.h"
#include "delay.h"
#include "text.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/                              
/* Private variables ---------------------------------------------------------*/
  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Global variables to set the written text color */
vu16 POINT_COLOR = 0x0000; 
vu16 BACK_COLOR = 0xFFFF;
u8 DeviceCode[6];
void DMA_M2MInit(void);
u16 FFT_Color[110];

/*******************************************************************************
* Function Name  : STM3210E_LCD_Init
* Description    : Initializes the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STM3210E_LCD_Init(void)
{ 
/* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();

/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();

  GPIO_SetBits(CtrlPort_RESET, CtrlPin_RESET);
  Delay_ms(10);
  GPIO_ResetBits(CtrlPort_RESET, CtrlPin_RESET);	 //复位LCD
  Delay_ms(50);
  GPIO_SetBits(CtrlPort_RESET, CtrlPin_RESET);
  Delay_ms(50);	

	LCD_WriteCom(0x0011);
	Delay_ms(100);
	LCD_WriteCom(0x0013);
	
	LCD_WriteCom(0x00D0);
	LCD_WriteData(0x0007);
	LCD_WriteData(0x0040); //41   
	LCD_WriteData(0x001c);//1e
	
	
	LCD_WriteCom(0x00D1);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x0018);
	LCD_WriteData(0x001d);
	
	LCD_WriteCom(0x00D2);
	LCD_WriteData(0x0001);
	LCD_WriteData(0x0011);//11
	
	LCD_WriteCom(0x00C0);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x003B);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x0002);
	LCD_WriteData(0x0011);
	
	LCD_WriteCom(0x00C1);
	LCD_WriteData(0x0010);
	LCD_WriteData(0x000B);
	LCD_WriteData(0x0088);
	
	LCD_WriteCom(0x00C5);
	LCD_WriteData(0x0001);
	
	LCD_WriteCom(0x00C8);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x0030); //30
	LCD_WriteData(0x0036);
	LCD_WriteData(0x0045);
	LCD_WriteData(0x0004);
	LCD_WriteData(0x0016);
	LCD_WriteData(0x0037);
	LCD_WriteData(0x0075);
	LCD_WriteData(0x0077);
	LCD_WriteData(0x0054);
	LCD_WriteData(0x000f);
	LCD_WriteData(0x0000);
	
	LCD_WriteCom(0x00E4);
	LCD_WriteData(0x00A0);
	
	LCD_WriteCom(0x00F0);
	LCD_WriteData(0x0001);
	
	LCD_WriteCom(0x00F3);
	LCD_WriteData(0x0040);
	LCD_WriteData(0x000A);
	
	LCD_WriteCom(0x00F7);
	LCD_WriteData(0x0080);
	
	LCD_WriteCom(0x0036);
	LCD_WriteData(0x000a);
	
	LCD_WriteCom(0x003A);
	LCD_WriteData(0x0055);
	
	/*wr_comm(0x00B3);
	wr_dat(0x0000);
	wr_dat(0x0000);
	wr_dat(0x0000);
	wr_dat(0x0011);*/
	
	LCD_WriteCom(0x002A);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x0001);
	LCD_WriteData(0x003F);
	
	LCD_WriteCom(0x002B);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x0000);
	LCD_WriteData(0x0001);
	LCD_WriteData(0x00df);
	
	Delay_ms(100);
	LCD_WriteCom(0x0029);
	//LCD_WriteCom(0x002C);
	ReadDeviceCode();
	DMA_M2MInit();
	LCD_Clear(Black);
    GPIO_SetBits(CtrlPort_BL, CtrlPin_BL);//开LCD背光
}

/*******************************************************************************
* Function Name  : LCD_SetTextColor
* Description    : Sets the Text color.
* Input          : - Color: specifies the Text color code RGB(5-6-5).
* Output         : - TextColor: Text color global variable used by LCD_DrawChar
*                  and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetTextColor(vu16 Color)
{
  POINT_COLOR = Color;
}

/*******************************************************************************
* Function Name  : LCD_SetBackColor
* Description    : Sets the Background color.
* Input          : - Color: specifies the Background color code RGB(5-6-5).
* Output         : - BackColor: Background color global variable used by 
*                  LCD_DrawChar and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetBackColor(vu16 Color)
{
  BACK_COLOR = Color;
}

/*******************************************************************************
* Function Name  : LCD_ClearLine
* Description    : Clears the selected line.
* Input          : - Line: the Line to be cleared.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_ClearLine(u16 Line)
{
   LCD_Fill(0, Line, 0x13F, Line+16, BACK_COLOR);
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Clears the hole LCD.
* Input          : Color: the color of the background.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(u16 Color)
{
  u32 index = 0;
  
  //设置窗口
  LCD_SetDisplayWindow(0, 0, 319, 479);
  //写GRAM颜色数据，全屏颜色Color
  for(index = 0; index < 153600; index++)
  {
    LCD->LCD_RAM = Color;
  }  
}


//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{                    
    u32 n;

	//设置窗口
	LCD_SetDisplayWindow(xsta, ysta, xend, yend);	
	 	   	   
	n=(u32)(yend-ysta+1)*(xend-xsta+1);
	    
	while(n--)
	LCD->LCD_RAM = color;       //显示所填充的颜色.     
}  
/*
//在指定区域内填充指定颜色
//区域大小:					   AlphaColor
//  (xend-xsta)*(yend-ysta)
void LCD_FillAlpha(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color,u16 appha)
{                    
    u16 n,m;
	u16 temp[480];
	
	    
	while(n--)
	LCD->LCD_RAM = color;       //显示所填充的颜色.     
}  
*/

/*******************************************************************************
* Function Name  : LCD_DrawChar
* Description    : Draws a character on LCD.
* Input          : - Xpos: the Line where to display the character shape.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - Ypos: start column address.
*                  - c: pointer to the character data.
* Output         : None
* Return         : None
*******************************************************************************/
u16 BGC_Buffer[16][16];
//在指定位置显示一个字符
//x:0~312
//y:0~464
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:bit1:叠加方式(1)还是非叠加方式(0)
//	   bit2:背景不是单色(1)背景是单色(0)
//	   bit7-4:仅不同行颜色变化(1)仅不同列颜色变化(2)行列都变化(4)		 	
void LCD_DisplayChar(u16 x,u16 y,u8 ASCII,u8 size,u8 mode)
{       
#define MAX_CHAR_POSX 312
#define MAX_CHAR_POSY 464 
    uc8 *temp;
    u8 pos, tc;
	      
    if(x>MAX_CHAR_POSX || y>MAX_CHAR_POSY)
	return;    
	if(ASCII<0x20 || ASCII>0x7E)
	ASCII = '?';
	else	   
	ASCII = ASCII-' ';//得到偏移后的值
	if(size == 12)
	temp = &(asc2_1206[ASCII][0]);  //调用1206字体
	else 
	{
		temp = &(asc2_1608[ASCII][0]);  //调用1608字体
		size=16;						//16*8以上字符仍显示为16*8
	
	}
	//设置窗口	
	LCD_SetDisplayWindow(x, y, x+(size>>1)-1, y+size-1);	

	if((mode&0x01) == 0) //非叠加方式
	{
		for(pos = 0; pos < size; pos++)
		{
			if(mode&0x10) BACK_COLOR=BGC_Buffer[pos][0];
			for(tc = 0; tc < size/2; tc++)
		    {                 
		        if(mode&0x40) BACK_COLOR=BGC_Buffer[pos][tc];
				else if(mode&0x20) BACK_COLOR=BGC_Buffer[0][tc];
				if((*temp) & (0x01<<tc))
				LCD_WriteData(POINT_COLOR);
				else 
				LCD_WriteData(BACK_COLOR);	         
		    }
			temp++;
		}	
	}else//叠加方式
	{
		for(pos = 0; pos < size; pos++)
		{
			for(tc = 0; tc < size/2; tc++)
		    {                 
		        if((*temp) & (0x01<<tc))
				LCD_DrawPoint(x+tc,y+pos);//画一个点     
		    }
			temp++;
		}
	}	    
}  


/*******************************************************************************
* Function Name  : LCD_DisplayStringLine
* Description    : Displays a string on the LCD.
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..19
*				   - Index: the position of the char which is displayed 
*							firstly	in this string.						 	
*                  - *ptr: pointer to string to display on LCD.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayStringLine(u16 Line, u16 Index, u8 *ptr)
{
  u16 refcolumn = Index<<3;

  /* Send the string character by character on lCD */
  while (*ptr != 0)
  {
    /* Display one character on LCD */
    LCD_DisplayChar(refcolumn, Line, *ptr, 16, 0);
    /* Decrement the column position by 8 */
	if(refcolumn > MAX_CHAR_POSX)
	{
		Line += 16;
		refcolumn = 0;
	}
	if(Line > MAX_CHAR_POSY)
	{
		Line = 0;
		refcolumn = 0;
		LCD_Clear(BACK_COLOR);
	}
	refcolumn += 8;
    /* Point on the next character */
    ptr++;
  }
}

//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//显示n个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{         	
	u8 t,temp;
	u8 enshow = 0;						   
	for(t = 0;t < len;t++)
	{
		temp = (num/mypow(10, len-t-1))%10;
		if(enshow==0 && t<(len-1))
		{
			if(temp == 0)
			{
				LCD_DisplayChar(x, y, ' ', size, mode);
				x += 8;
				continue;
			}
			else 
			enshow = 1; 		 	 
		}
	 	LCD_DisplayChar(x, y, temp+'0', size, mode); 
		x += 8;
	}
} 
//显示2个数字
//x,y:起点坐标
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~99);	 
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 size,u8 mode)
{         	
	LCD_DisplayChar(x,y,(num/10)%10+'0',size,mode); 
    LCD_DisplayChar(x+size/2,y,num%10+'0',size,mode);
} 

//显示3个数字
//x,y:起点坐标
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~999);
void LCD_Show3Num(u16 x,u16 y,u16 num,u8 size,u8 mode)
{   
	LCD_DisplayChar(x,y,(num/100)%10+'0',size,mode);      							   
    LCD_DisplayChar(x+size/2,y,(num/10)%10+'0',size,mode); 
    LCD_DisplayChar(x+size,y,num%10+'0',size,mode); 
} 

/*******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Displays a line.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Length: line length.
*                  - Direction: line direction.
*                    This parameter can be one of the following values: Vertical 
*                    or Horizontal.
* Output         : None
* Return         : None
*******************************************************************************/
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 			   
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 

	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	} 
}    

/*******************************************************************************
* Function Name  : LCD_DrawRect
* Description    : Displays a rectangle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Height: display rectangle height.
*                  - Width: display rectangle width.
* Output         : None
* Return         : None
*******************************************************************************/
//画矩形
void LCD_DrawRect(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_Fill(x1,y1,x2,y1,POINT_COLOR);
	LCD_Fill(x1,y1,x1,y2,POINT_COLOR);
	LCD_Fill(x1,y2,x2,y2,POINT_COLOR);
	LCD_Fill(x2,y1,x2,y2,POINT_COLOR);
}

/*******************************************************************************
* Function Name  : LCD_DrawCircle
* Description    : Displays a circle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Height: display rectangle height.
*                  - Width: display rectangle width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawCircle(u16 Xpos, u16 Ypos, u16 Radius)
{
  s32  D;/* Decision Variable */ 
  u32  CurX;/* Current X Value */
  u32  CurY;/* Current Y Value */ 
  u8   mark = 0;
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY)
  {
	LCD_DrawPoint(Xpos + CurX, Ypos - CurY);
	LCD_DrawPoint(Xpos - CurX, Ypos - CurY);
   	LCD_DrawPoint(Xpos + CurY, Ypos - CurX);
	LCD_DrawPoint(Xpos - CurY, Ypos - CurX);
	if(mark)
	{
		LCD_DrawPoint(Xpos + CurX, Ypos - CurY - 1);
		LCD_DrawPoint(Xpos - CurX, Ypos - CurY - 1);	
	   	LCD_DrawPoint(Xpos + CurY + 1, Ypos - CurX);
		LCD_DrawPoint(Xpos - CurY - 1, Ypos - CurX);
		LCD_DrawPoint(Xpos + CurX - 1, Ypos - CurY);
		LCD_DrawPoint(Xpos - CurX + 1, Ypos - CurY);	
	   	LCD_DrawPoint(Xpos + CurY, Ypos - CurX + 1);
		LCD_DrawPoint(Xpos - CurY, Ypos - CurX + 1);
	}
	    		
	LCD_DrawPoint(Xpos + CurY, Ypos + CurX);
	LCD_DrawPoint(Xpos - CurY, Ypos + CurX);
	if(mark)
	{
		LCD_DrawPoint(Xpos + CurY + 1, Ypos + CurX);
		LCD_DrawPoint(Xpos - CurY - 1, Ypos + CurX);
		LCD_DrawPoint(Xpos + CurY, Ypos + CurX - 1);
		LCD_DrawPoint(Xpos - CurY, Ypos + CurX - 1);
	}

	LCD_DrawPoint(Xpos + CurX, Ypos + CurY);	   
    LCD_DrawPoint(Xpos - CurX, Ypos + CurY);
	if(mark)
	{
		LCD_DrawPoint(Xpos + CurX, Ypos + CurY + 1);	   
	    LCD_DrawPoint(Xpos - CurX, Ypos + CurY + 1);
		LCD_DrawPoint(Xpos + CurX - 1, Ypos + CurY);	   
	    LCD_DrawPoint(Xpos - CurX + 1, Ypos + CurY);
	}
	
	mark=0;	    
   
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
	  mark = 1;
    }
    CurX++;
  }
}

void LCD_FillCircle(u16 Xpos, u16 Ypos, u16 Radius)
{
  s32  D;/* Decision Variable */ 
  u32  CurX;/* Current X Value */
  u32  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY)
  {
	LCD_Fill(Xpos-CurX, Ypos-CurY, Xpos+CurX, Ypos-CurY, POINT_COLOR);
	LCD_Fill(Xpos-CurY, Ypos-CurX, Xpos+CurY, Ypos-CurX, POINT_COLOR);
	LCD_Fill(Xpos-CurY, Ypos+CurX, Xpos+CurY, Ypos+CurX, POINT_COLOR);
	LCD_Fill(Xpos-CurX, Ypos+CurY, Xpos+CurX, Ypos+CurY, POINT_COLOR);
	  
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}
//指定某行填充指定颜色.
//消掉第x列的y行
void Drow_Line_FFT(u16 x,u16 y,u16 color)
{
	u8 t;
	LCD_SetDisplayWindow(x, 188-y, x+15, 188-y); 			   										     
	for(t=0;t<16;t++) LCD->LCD_RAM = color;				    
} 
//顶点/高端处理
//*flyval:当前顶点位置(14个)
//topstay1:用来标记在0~6顶点是否需要停留
//topstay2:用来标记在7~13顶点是否需要停留
void Top_Delet_FFT(u8 *flyval,u32 *topstay1,u32 *topstay2)
{
	u8 t;
	u32 temp;
	u32 temp1;
	temp=*topstay1;
	BACK_COLOR=BLACK;//背景黑色   
	for(t=0;t<14;t++)
	{   
		if(t==7)temp=*topstay2;
		temp1=temp&0x0F;
		if(temp1==0)//不需要停留
		{	 
			Drow_Line_FFT(13+t*19,flyval[t],0x28CA);
			if(flyval[t])flyval[t]--;
			Drow_Line_FFT(13+t*19,flyval[t],0x5B16);
		}else 
		{	
			temp1--;
			if(t<7) 
			{
				*topstay1&=~(0X0F<<4*t);
				*topstay1|=temp1<<4*t;//取消停留标志
			}else
			{
				*topstay2&=~(0X0F<<4*(t-7));
				*topstay2|=temp1<<4*(t-7);//取消停留标志
			}
		}	 							
		temp>>=4;	  
	} 
}  
//升高频谱图
//10*127大小的频谱
//显示从y-->top 这么高的柱形
void TFT_ShowFFT(u8* curv,u8* topv,u8 *flyv,u32 *toppos1,u32 *toppos2)
{			    
	u8 t1,n,t,ts;
	u16 temp;	 

	LCD_WriteCom(0x0036);LCD_WriteData(0x008a);	//扫描方向控制 左至右,下至上  
	for(ts=0;ts<14;ts++)
	{	
		if(*curv>*topv)//需要更新 
		{
			t1=*curv-*topv;//要升高这么多 
			temp=188-*topv;

			//10*50大小	   	   
			LCD_SetDisplayWindow(13+ts*19, temp-t1, 13+ts*19+15, temp); 	     
			//显示有效数据
			for(n=0;n<t1;n++)
			{	
				POINT_COLOR=FFT_Color[*topv+n];					    					    
				for(t=0;t<16;t++) LCD->LCD_RAM = POINT_COLOR; 
			} 
			*topv=*curv;  
		}
		topv++;
		curv++;
	} 		  
	topv-=14; //偏移回原来的位置
	for(ts=0;ts<14;ts++)
	{
		if(*flyv<=*topv)//处理当前柱的顶点是不是超过了先前柱的顶点,超过了则重置顶点
		{
		//	POINT_COLOR=FFT_Color[*flyv+n];	 
		//	Drow_Line_FFT(ts*17,*flyv,POINT_COLOR);//把顶点去掉
			*flyv=*topv+1;
			Drow_Line_FFT(13+ts*19,*flyv,0x5B16);	//重新置位顶点
			if(ts<7)*toppos1|=0x0f<<ts*4; 
			else *toppos2|=0x0f<<(ts-7)*4;
		}
		flyv++;
		topv++; 
	}   
	topv-=14; //偏移回原来的位置
	for(ts=0;ts<14;ts++) //有规律的消除一格
	{		 
		Drow_Line_FFT(13+ts*19,*topv,0x28CA);//背景黑色	
		if(*topv)(*topv)--;   
		topv++;
	}
	LCD_WriteCom(0x0036);LCD_WriteData(0x000a);  									    
}
  
//MP3的进度条
//显示进度条
void TFT_ProBar(u16 lenth)
{                    
    u32 n;				 	  
	u16 color;
	if(lenth>280)return;
   	LCD_WriteCom(0x0036);LCD_WriteData(0x002a);
	LCD_SetDisplayWindow(196,20,196+8-1,20+280-1); 
	for(n=0;n<2240;n++)
	{											  
		if(n<8*lenth)color=0x54DA;
		else color=0x52B0;
		LCD->LCD_RAM = color;//显示所填充的颜色. 
	}
	LCD_WriteCom(0x0036);LCD_WriteData(0x000a);
}
 
u16 BGV_Color[1014];
//MP3音量显示
void TFT_Volume(u16 volume)
{
	u8 n,m,k;
	if(volume>25) volume = 25;
	POINT_COLOR = GradualColor(0xFFFF,0x0000,200);
	LCD_WriteCom(0x0036);LCD_WriteData(0x002a);
	LCD_SetDisplayWindow(VOLBAR_Y-12,VOLBAR_X,VOLBAR_Y,VOLBAR_X+77);
	
	for(n=0;n<26;n++)
	{
		if(n<=volume)
		{
			for(m=0;m<3;m++)
			for(k=0;k<13;k++)
			{
				if(13-k>n/2 || m==2)
				{
					if(n<=1&&k==12&&m!=2)
					LCD->LCD_RAM = GradualColor(BGV_Color[n*39+m*13+k],POINT_COLOR,300);
					else
					LCD->LCD_RAM = BGV_Color[n*39+m*13+k];
				}
				else
				{
					if(13-k==n/2)
					{
						if(n%2==0)
						LCD->LCD_RAM = GradualColor(BGV_Color[n*39+m*13+k],POINT_COLOR,400);
						else
						LCD->LCD_RAM = GradualColor(BGV_Color[n*39+m*13+k],POINT_COLOR,700);
					}
					else
					LCD->LCD_RAM = POINT_COLOR;
				}
			}
		}
		else
		{
			for(m=0;m<39;m++)
			LCD->LCD_RAM = BGV_Color[n*39+m];
		}		
	}
	LCD_WriteCom(0x0036);LCD_WriteData(0x000a);
	if(volume*3<6) n=6;
	else if(volume*3>71) n=71;
	else n=volume*3;
	TFT_DrawCButton(VOLBAR_X+n,VOLBAR_Y-6,6,0xACF9,0x2910,0);		
} 

void TFT_DrawSpeak(u16 Xpos,u16 Ypos)
{
	u16 temp[90];
	LCD_ReadRect(Xpos,Ypos-4,Xpos+2,Ypos+4,temp);
	POINT_COLOR=GradualColor(0xFFFF,0x0000,150);
	TFT_DrawTriangle(Xpos+13,Ypos,14,1,POINT_COLOR);
	LCD_Fill(Xpos,Ypos-4,Xpos+6,Ypos+4,POINT_COLOR);
	LCD_WriteTempRect(Xpos,Ypos-4,Xpos+2,Ypos+4,temp);
	LCD_ReadRect(Xpos+14,Ypos-7,Xpos+19,Ypos+7,temp);
	LCD_DrawCircle(Xpos+18,Ypos,7);
	LCD_WriteTempRect(Xpos+14,Ypos-7,Xpos+19,Ypos+7,temp);
	LCD_FillCircle(Xpos+18,Ypos,2);
}

//渐变色
//从scolor渐变到ecolor,渐变%per
u16 GradualColor(u16 scolor,u16 ecolor,u16 per)
{
	s32 sr,sg,sb;
	s32 er,eg,eb;

	if(per>1000) per=1000;
	
	sr=(scolor>>11)&0x001F; er=(ecolor>>11)&0x001F;
	sg=(scolor>>5)&0x003F; eg=(ecolor>>5)&0x003F;
	sb=scolor&0x001F; eb=ecolor&0x001F;

	sr += (er-sr)*per/1000;
	sg += (eg-sg)*per/1000;
	sb += (eb-sb)*per/1000;

	sr <<= 11; sg <<= 5;
	return (u16)(sr|sg|sb);
}

u16 AlphaColor(u16 fcolor,u16 bcolor,u16 alpha)
{
	s32 fr,fg,fb;
	s32 br,bg,bb;
	if(alpha>1000) alpha=1000;
	
	fr=(fcolor>>11)&0x001F; br=(bcolor>>11)&0x001F;
	fg=(fcolor>>5)&0x003F; bg=(bcolor>>5)&0x003F;
	fb=fcolor&0x001F; bb=bcolor&0x001F;

	fr = fr*alpha/1000 + br*(1000-alpha)/1000;
	fg = fg*alpha/1000 + bg*(1000-alpha)/1000;
	fb = fb*alpha/1000 + bb*(1000-alpha)/1000;

	fr <<= 11; fg <<= 5;
	return (u16)(fr|fg|fb);
}
//			  0	  1		2	  3		4	  5		6	   7	  8		 9		10
u32 SinTab[]={0,17452,34899,52335,69756,87155,104528,121869,139173,156434,173648};
//mode:0-进度圆点，1-播放，2-暂停，3-上一曲，4-下一曲
void TFT_DrawCButton(u16 Xpos, u16 Ypos, u16 Radius, u16 Color1, u16 Color2, u8 mode)
{
  s32  D;/* Decision Variable */ 
  u32  CurX;/* Current X Value */
  u32  CurY;/* Current Y Value */
  u16  temp;
  u16  GraPer;
  u16  tt = Radius-Radius/9;
  u16  tPer1 = 1800/Radius;
  u16  tPer2 = 600/Radius; 
  u16  tPoint;
  u16  tLimit;
  u16  tPerStep1 = 500/Radius;
  u16  tPerStep2;
  u16  tRadius = Radius;
  u8   mark=0;
  							
  for(;tRadius>0;tRadius--)
  {
	  D = 3 - (tRadius << 1);
	  CurX = 0;
	  CurY = tRadius;	
						
	  if(tRadius>tt)
	  temp = GradualColor(Color1,0xFFFF,(tRadius-tt)*tPer1);
	  else	 
	  temp = GradualColor(Color2,Color1,(tRadius+Radius/10)*tPer2);	//圆形按钮上半部分的颜色渐变
	  temp = AlphaColor(0xB30D,temp,200);
	  POINT_COLOR = temp;
	  tPoint = temp;	  //渐变色缓存
	  tLimit = (u32)tRadius*SinTab[tRadius*8/Radius]/1000000;
	  tPerStep2 = (u32)tRadius*1570796/1000000;
	  tPerStep2 = 500/tPerStep2;
	  GraPer = 500;
						   
	  while (CurX <= CurY)
	  {											  
	    POINT_COLOR = tPoint;				
		
		LCD_DrawPoint(Xpos + CurX, Ypos - CurY);
		LCD_DrawPoint(Xpos - CurX, Ypos - CurY);
	   	LCD_DrawPoint(Xpos + CurY, Ypos - CurX);
		LCD_DrawPoint(Xpos - CurY, Ypos - CurX);
		if(mark)
		{
			LCD_DrawPoint(Xpos + CurX, Ypos - CurY - 1);
			LCD_DrawPoint(Xpos - CurX, Ypos - CurY - 1);	
		   	LCD_DrawPoint(Xpos + CurY + 1, Ypos - CurX);
			LCD_DrawPoint(Xpos - CurY - 1, Ypos - CurX);
			LCD_DrawPoint(Xpos + CurX - 1, Ypos - CurY);
			LCD_DrawPoint(Xpos - CurX + 1, Ypos - CurY);	
		   	LCD_DrawPoint(Xpos + CurY, Ypos - CurX + 1);
			LCD_DrawPoint(Xpos - CurY, Ypos - CurX + 1);
		}
		    		
		if(CurX>=tLimit)
		{
	  		if(tRadius>Radius*2/3)
			temp = GradualColor(Color2,0xFFFF,500-GraPer);
			else
			temp = GradualColor(Color2,0xFFFF,(CurX+1)*tPerStep1);
			temp = AlphaColor(0xFFFF,temp,150);
			POINT_COLOR = temp;
		}	
		LCD_DrawPoint(Xpos + CurY, Ypos + CurX);
		LCD_DrawPoint(Xpos - CurY, Ypos + CurX);
		if(mark)
		{
			LCD_DrawPoint(Xpos + CurY + 1, Ypos + CurX);
			LCD_DrawPoint(Xpos - CurY - 1, Ypos + CurX);
			LCD_DrawPoint(Xpos + CurY, Ypos + CurX - 1);
			LCD_DrawPoint(Xpos - CurY, Ypos + CurX - 1);
		}

		if(tRadius>Radius*2/3)
		{
			temp = GradualColor(Color2,0xFFFF,GraPer);
			GraPer -= tPerStep2;
		}
		else
	  	temp = GradualColor(Color2,0xFFFF,CurY*tPerStep1);
		temp = AlphaColor(0xFFFF,temp,150);
		POINT_COLOR = temp;
		LCD_DrawPoint(Xpos + CurX, Ypos + CurY);	   
	    LCD_DrawPoint(Xpos - CurX, Ypos + CurY);
		if(mark)
		{
			LCD_DrawPoint(Xpos + CurX, Ypos + CurY + 1);	   
		    LCD_DrawPoint(Xpos - CurX, Ypos + CurY + 1);
			LCD_DrawPoint(Xpos + CurX - 1, Ypos + CurY);	   
		    LCD_DrawPoint(Xpos - CurX + 1, Ypos + CurY);
		}
		
		mark=0;	    
	    	   	    	   	   										  
	    if (D < 0)
	    { 
	      D += (CurX << 2) + 6;
	    }
	    else
	    {
	      D += ((CurX - CurY) << 2) + 10;
	      CurY--;
		  mark=1;
	    }
	    CurX++;
	  }
	  if(tRadius%2)
	  {
		  CurX--;
		  POINT_COLOR = tPoint;
		  LCD_DrawPoint(Xpos + CurX, Ypos - CurY);
		  LCD_DrawPoint(Xpos - CurX, Ypos - CurY);
		  LCD_DrawPoint(Xpos + CurY, Ypos - CurX);
		  LCD_DrawPoint(Xpos - CurY, Ypos - CurX);
		  temp = GradualColor(Color2,0xFFFF,CurY*tPerStep1);
		  temp = AlphaColor(0xFFFF,temp,150);
		  POINT_COLOR = temp;
		  LCD_DrawPoint(Xpos + CurY, Ypos + CurX);
		  LCD_DrawPoint(Xpos - CurY, Ypos + CurX);
		  LCD_DrawPoint(Xpos + CurX, Ypos + CurY);	   
		  LCD_DrawPoint(Xpos - CurX, Ypos + CurY);		  
		  
		  CurY++;CurX++;
		  POINT_COLOR = tPoint;
		  LCD_DrawPoint(Xpos + CurX, Ypos - CurY);
		  LCD_DrawPoint(Xpos - CurX, Ypos - CurY);
		  LCD_DrawPoint(Xpos + CurY, Ypos - CurX);
		  LCD_DrawPoint(Xpos - CurY, Ypos - CurX);
		  temp = GradualColor(Color2,0xFFFF,CurY*tPerStep1);
		  temp = AlphaColor(0xFFFF,temp,150);
		  POINT_COLOR = temp;
		  LCD_DrawPoint(Xpos + CurY, Ypos + CurX);
		  LCD_DrawPoint(Xpos - CurY, Ypos + CurX);
		  LCD_DrawPoint(Xpos + CurX, Ypos + CurY);	   
		  LCD_DrawPoint(Xpos - CurX, Ypos + CurY);
	  }
   }		
   POINT_COLOR = GradualColor(0xFFFF,0x0000,200);
   switch(mode)
   {
    	case 0:break;
		case 1:
			TFT_DrawTriangle(Xpos-(Radius*5/12),Ypos,Radius,0,POINT_COLOR);
			break;
		case 2:
			tRadius = Radius/5;
			temp = Radius/2;
			Radius = Radius/3;			 
			LCD_Fill(Xpos-Radius,Ypos-temp,Xpos-Radius+tRadius-1,Ypos+temp,POINT_COLOR);
			LCD_Fill(Xpos+Radius-tRadius+1,Ypos-temp,Xpos+Radius,Ypos+temp,POINT_COLOR);
			break;
		case 3:
			TFT_DrawTriangle(Xpos+(Radius/2)-(Radius/8),Ypos,Radius>>1,1,POINT_COLOR);
			TFT_DrawTriangle(Xpos-(Radius/8),Ypos,Radius>>1,1,POINT_COLOR);
			break;
		case 4:
			TFT_DrawTriangle(Xpos-(Radius/2)+(Radius/8),Ypos,Radius>>1,0,POINT_COLOR);
			TFT_DrawTriangle(Xpos+(Radius/8),Ypos,Radius>>1,0,POINT_COLOR);
			break;
   }	
}

void TFT_DrawCirque(u16 Xpos,u16 Ypos,u16 ORadius,u16 IRadius,u16 Color)
{
	u16 n;
	POINT_COLOR = Color;
	for(n=IRadius;n<=ORadius;n++)
	LCD_DrawCircle(Xpos,Ypos,n);	
}
//dir:0->,1-<
void TFT_DrawTriangle(u16 Xpos,u16 Ypos,u16 Height,u8 dir,u16 Color)
{
	u16 i;
	u16 y;
	y = (u32)Height*57735/100000;
	for(i=0;i<Height;i++)
	{
		if(y !=	(u32)(Height-i)*57735/100000)
		y--;
		LCD_Fill(Xpos,Ypos-y,Xpos,Ypos+y,Color);
		if(dir)
		Xpos--;
		else
		Xpos++;
	}
}

void TFT_CloseButton(u16 Xpos, u16 Ypos, u16 Width, u16 color1, u16 color2)
{
	u16 t;
	u16 temp;
	u16 GraPer;
	u16 step=1000/Width;
	for(t=0;t<Width;t++)
	{
		GraPer=t*step;
		temp=GradualColor(color1,color2,GraPer);
		LCD_Fill(Xpos,Ypos+t,Xpos+Width-1,Ypos+t,temp); 
	}
	POINT_COLOR=GradualColor(color2,0x0000,300);
	LCD_DrawRect(Xpos-1,Ypos-1,Xpos+Width,Ypos+Width);
	temp=GradualColor(color1,0xFFFF,300);
	LCD_Fill(Xpos,Ypos,Xpos+Width-1,Ypos,temp);
	POINT_COLOR=GradualColor(0xFFFF,0x0000,300);
	temp=Width/4;
	LCD_DrawLine(Xpos+temp,Ypos+temp,Xpos+Width-temp,Ypos+Width-temp);
	LCD_DrawLine(Xpos+temp,Ypos+temp+1,Xpos+Width-temp,Ypos+Width-temp+1);
	LCD_DrawLine(Xpos+Width-temp,Ypos+temp,Xpos+temp,Ypos+Width-temp);
	LCD_DrawLine(Xpos+Width-temp,Ypos+temp+1,Xpos+temp,Ypos+Width-temp+1);
}

void TFT_FucButton(u16 Xpos, u16 Ypos, u16 Width, u16 color1, u16 color2, u8 *str)
{
	u16 t;
	u16 temp;
	u16 GraPer;
	u16 step=2000/Width;
	for(t=0;t<=Width/2;t++)
	{
		GraPer=t*step;
		temp=GradualColor(color1,color2,GraPer);
		LCD_Fill(Xpos,Ypos+t,Xpos+Width-1,Ypos+t,temp); 
	}
	temp=GradualColor(color2,0x0000,400);
	LCD_Fill(Xpos,Ypos+Width*3/10,Xpos+Width-1,Ypos+Width/2,temp);
	temp=GradualColor(color1,0xFFFF,300);
	LCD_Fill(Xpos,Ypos,Xpos+Width-1,Ypos,temp);
	POINT_COLOR=GradualColor(0xFFFF,0x0000,150);
	Show_Str_Mid(Xpos,Ypos+(Width/2-12)/2,str,12,Width);
}
///*******************************************************************************
//* Function Name  : LCD_DrawMonoPict
//* Description    : Displays a monocolor picture.
//* Input          : - Pict: pointer to the picture array.
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void LCD_DrawMonoPict(uc32 *Pict)
//{
//  u32 index = 0, i = 0;
//
//  LCD_SetCursor(0, 319); 
//
//  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
//  for(index = 0; index < 2400; index++)
//  {
//    for(i = 0; i < 32; i++)
//    {
//      if((Pict[index] & (1 << i)) == 0x00)
//      {
//        LCD_WriteRAM(BackColor);
//      }
//      else
//      {
//        LCD_WriteRAM(TextColor);
//      }
//    }
//  }
//}
//
///*******************************************************************************
//* Function Name  : LCD_WriteBMP
//* Description    : Displays a bitmap picture loaded in the internal Flash.
//* Input          : - BmpAddress: Bmp picture address in the internal Flash.
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void LCD_WriteBMP(u32 BmpAddress)
//{
//  u32 index = 0, size = 0;
//
//  /* Read bitmap size */
//  size = *(vu16 *) (BmpAddress + 2);
//  size |= (*(vu16 *) (BmpAddress + 4)) << 16;
//
//  /* Get bitmap data address offset */
//  index = *(vu16 *) (BmpAddress + 10);
//  index |= (*(vu16 *) (BmpAddress + 12)) << 16;
//
//  size = (size - index)/2;
//
//  BmpAddress += index;
//
//  /* Set GRAM write direction and BGR = 1 */
//  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
//  /* AM=1 (address is updated in vertical writing direction) */
//  LCD_WriteReg(R3, 0x1008);
// 
//  LCD_WriteRAM_Prepare();
// 
//  for(index = 0; index < size; index++)
//  {
//    LCD_WriteRAM(*(vu16 *)BmpAddress);
//    BmpAddress += 2;
//  }
// 
//  /* Set GRAM write direction and BGR = 1 */
//  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
//  /* AM = 1 (address is updated in vertical writing direction) */
//  LCD_WriteReg(R3, 0x1030);
//}
//

/*******************************************************************************
* Function Name  : LCD_DrawPoint
* Description    : Draw a point on LCD.
* Input          : - Xpos: specifies the X position where to displays the point.
*                  - Ypos: specifies the Y position where to displays the point. 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawPoint(u16 Xpos,u16 Ypos)
{
	LCD_SetDisplayWindow(Xpos, Ypos, Xpos, Ypos);
    LCD->LCD_RAM = POINT_COLOR;
}

/*******************************************************************************
* Function Name  : LCD_SetDisplayWindow
* Description    : Sets a display window
* Input          : - Xpos: specifies the X buttom left position.
*                  - Ypos: specifies the Y buttom left position.
*                  - Height: display window height.
*                  - Width: display window width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetDisplayWindow(u16 Xsta, u16 Ysta, u16 Xend, u16 Yend)
{
  /* 设置窗口 */
    LCD->LCD_REG = 0x002A;		   //设定列地址
  	LCD->LCD_RAM = Xsta>>8;	       //开始X
  	LCD->LCD_RAM = Xsta&0x00FF;	   //开始X
  	LCD->LCD_RAM = Xend>>8;	       //结束X
  	LCD->LCD_RAM = Xend&0x00FF;	   //结束X
  	LCD->LCD_REG = 0x002B;		   //设定页地址
  	LCD->LCD_RAM = Ysta>>8;		   //开始Y
  	LCD->LCD_RAM = Ysta&0x00FF;	   //开始Y
  	LCD->LCD_RAM = Yend>>8;	       //结束Y
  	LCD->LCD_RAM = Yend&0x00FF;	   //结束Y
  	LCD->LCD_REG = 0x002C; 		   //开始写窗口数据		 
}

/*******************************************************************************
* Function Name  : LCD_WriteCom
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteCom(u16 LCD_Reg)
{
  /* Write 16-bit Reg Index*/
  LCD->LCD_REG = LCD_Reg;
}

void LCD_WriteData(u16 LCD_Data)
{
   /* Write 16-bit Reg Data*/
   LCD->LCD_RAM = LCD_Data;
}
/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
u16 LCD_ReadPoint(u16 x,u16 y)
{
  u16 temp,r,g,b;
  /* 设置窗口 */
  LCD->LCD_REG = 0x002A;		   //设定列地址
  LCD->LCD_RAM = x>>8;	       //开始X
  LCD->LCD_RAM = x&0x00FF;	   //开始X
  LCD->LCD_RAM = x>>8;	       //结束X			   
  LCD->LCD_RAM = x&0x00FF;	   //结束X
  LCD->LCD_REG = 0x002B;		   //设定页地址
  LCD->LCD_RAM = y>>8;		   //开始Y
  LCD->LCD_RAM = y&0x00FF;	   //开始Y
  LCD->LCD_RAM = y>>8;	       //结束Y
  LCD->LCD_RAM = y&0x00FF;	   //结束Y
  LCD->LCD_REG = 0x002E; 		   //开始写窗口数据
  temp = LCD->LCD_RAM;
  /* Read 16-bit Reg Data*/
  temp = LCD->LCD_RAM;								  
  r = temp&0x001F; g = temp&0x07E0; b = temp&0xF800;
  r <<= 11; b >>= 11; temp = r|g|b;
  return (temp);
}
							 
/*******************************************************************************
* Function Name  : LCD_ReadRect
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
void LCD_ReadRect(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *buff)
{
  u16 temp,r,g,b;
  u32 n;
 /* 设置窗口 */
  LCD->LCD_REG = 0x002A;		   //设定列地址
  LCD->LCD_RAM = xsta>>8;	       //开始X
  LCD->LCD_RAM = xsta&0x00FF;	   //开始X
  LCD->LCD_RAM = xend>>8;	       //结束X
  LCD->LCD_RAM = xend&0x00FF;	   //结束X
  LCD->LCD_REG = 0x002B;		   //设定页地址
  LCD->LCD_RAM = ysta>>8;		   //开始Y
  LCD->LCD_RAM = ysta&0x00FF;	   //开始Y
  LCD->LCD_RAM = yend>>8;	       //结束Y
  LCD->LCD_RAM = yend&0x00FF;	   //结束Y
  LCD->LCD_REG = 0x002E; 		   //开始读窗口数据
  temp = LCD->LCD_RAM;
  n=(u32)(yend-ysta+1)*(xend-xsta+1);
  while(n--)
  {
	  /* Read 16-bit Reg Data*/
	  temp = LCD->LCD_RAM;
	  r = temp&0x001F; g = temp&0x07E0; b = temp&0xF800;
	  r <<= 11; b >>= 11;
	  *buff = r|g|b;
	  buff++;
  }
}

void LCD_WriteTempRect(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *buff)
{
	u32 n;
	LCD_SetDisplayWindow(xsta, ysta, xend, yend);
	n=(u32)(yend-ysta+1)*(xend-xsta+1);
	while(n--)
	{
	  /* Write 16-bit Reg Data*/
	  LCD->LCD_RAM = *buff;
	  buff++;
	}
}

u16 LCD_ReadData(void)
{
  /* Read 16-bit Reg Data*/
  return (LCD->LCD_RAM);
}


///*******************************************************************************
//* Function Name  : LCD_WriteRAM_Prepare
//* Description    : Prepare to write to the LCD RAM.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void LCD_WriteRAM_Prepare(void)
//{
//  LCD->LCD_REG = 0x002C;
//}
//
///*******************************************************************************
//* Function Name  : LCD_WriteRAM
//* Description    : Writes to the LCD RAM.
//* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void LCD_WriteRAM(u16 RGB_Code)
//{
//  /* Write 16-bit GRAM Reg */
//  LCD->LCD_RAM = RGB_Code;
//}


///*******************************************************************************
//* Function Name  : LCD_PowerOn
//* Description    : Power on the LCD.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void LCD_PowerOn(void)
//{
// /* Power On sequence -------------------------------------------------------*/ 
//    LCD_WriteReg(R16, 0x0000);  /* SAP, BT[3:0], AP, DSTB, SLP, STB */ 
//    LCD_WriteReg(R17, 0x0000);  /* DC1[2:0], DC0[2:0], VC[2:0] */ 
//    LCD_WriteReg(R18, 0x0000);  /* VREG1OUT voltage */ 
//    LCD_WriteReg(R19, 0x0000);  /* VDV[4:0] for VCOM amplitude */ 
//    Delay_ms(200);                /* Dis-charge capacitor power voltage (200ms) */   
//    LCD_WriteReg(R17, 0x0007);  /* DC1[2:0], DC0[2:0], VC[2:0] */ 
//    Delay_ms(50);                 /* Delay 50 ms */ 
//    LCD_WriteReg(R16, 0x12B0);  /* SAP, BT[3:0], AP, DSTB, SLP, STB */ 
//    Delay_ms(50);                  /* Delay 50 ms */ 
//    LCD_WriteReg(R18, 0x01BD);  /* External reference voltage= Vci */ 
//    Delay_ms(50);                 /* Delay 50 ms */  
//    LCD_WriteReg(R19, 0x1400);       /* VDV[4:0] for VCOM amplitude */ 
//    LCD_WriteReg(R41, 0x000E);  /* VCM[4:0] for VCOMH */ 
//    Delay_ms(50);                 /* Delay 50 ms */ 
//    LCD_WriteReg(R7, 0x0173);  /* 262K color and display ON */
//}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : Enables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOn(void)
{
  /* Display On */
  LCD_WriteCom(0x0029); /* 262K color and display ON */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOff
* Description    : Disables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOff(void)
{
  /* Display Off */
  LCD_WriteCom(0x0028); 
}


void ReadDeviceCode(void)
{
	u8 i;
	LCD_WriteCom(0x00BF);
	for(i=0; i<6; i++)
	{
		DeviceCode[i] = (u8)(LCD_ReadData());
	}
}

/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | \
                         RCC_APB2Periph_GPIOB, ENABLE);
  						 

  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate 
     function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | \
  								GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | \
								GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;	
                                
                                
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | \
  								GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | \
                                GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
                                
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Set PD.7(NE1 (LCD/CS)) PD.11(A16 (RS)) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_11;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* Control Pin PB.02(LCD_RESET) as general function push pull*/
  /* Control Pin PB.08(LCD_BACKLIGHT) as general function push pull*/
  GPIO_InitStructure.GPIO_Pin = CtrlPin_RESET;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(CtrlPort_RESET, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = CtrlPin_BL;
  GPIO_Init(CtrlPort_BL, &GPIO_InitStructure);
  GPIO_SetBits(CtrlPort_RESET, CtrlPin_RESET);
  GPIO_ResetBits(CtrlPort_BL, CtrlPin_BL);
}

/*******************************************************************************
* Function Name  : LCD_FSMCConfig
* Description    : Configures the Parallel interface (FSMC) for LCD(Parallel mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;

/*-- FSMC Configuration ------------------------------------------------------*/
  /* FSMC_Bank1_NORSRAM4 timing configuration */
  p.FSMC_AddressSetupTime = 1;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 2;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  /* FSMC_Bank1_NORSRAM4 configured as follows:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Disable
        - Asynchronous Wait = Disable */
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsyncWait = FSMC_AsyncWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  

  /* Enable FSMC_Bank1_NORSRAM4 */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void DMA_M2MInit(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)0;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
}

void DMA_M2MStart(u32 SRCAddr, u32 DSTAddr, u32 num)
{
  DMA1_Channel6->CPAR = SRCAddr;
  DMA1_Channel6->CMAR = DSTAddr;
  DMA1_Channel6->CNDTR = num;
  DMA_Cmd(DMA1_Channel6, ENABLE);
  while(DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET);
  DMA_ClearFlag(DMA1_FLAG_TC6);
  DMA_Cmd(DMA1_Channel6, DISABLE);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
