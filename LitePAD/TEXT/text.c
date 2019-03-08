#include "lcd.h"
#include "text.h"
#include "fontupd.h"
#include "flash.h"	  
//Mini STM32开发板
//文本显示程序 V1.1					  
//正点原子@ALIENTEK
//2010/5/23		

#define NumWordPerSec 94
#define StartSec 0xA1
#define StartIndex 0xA1
#define EndSec 0xF7
#define EndIndex 0xFE 

		   
//////////////////////////////////////////////常用函数////////////////////////////////////////                      
//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,ascii码
//mat  数据存放地址 size*2 bytes大小
//正点原子@HYW
//CHECK:09/10/30
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset;
	unsigned short int matnum;

	qh=*code;
	ql=*(++code);
	matnum = (size*size)>>3;
	if(qh<StartSec||ql<StartIndex||ql>EndIndex||qh>EndSec)//非 常用汉字
	{   		    
	    for(i=0;i<matnum;i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}          

	foffset=((qh-StartSec)*NumWordPerSec+(ql-StartIndex))*matnum;//得到字库中的字节偏移量  		  

	if(size==16) SPI_Flash_Read(mat, foffset+FONT16ADDR, matnum);
	else if(size==12) SPI_Flash_Read(mat, foffset+FONT12ADDR, matnum);
	else SPI_Flash_Read(mat, foffset+FONT24ADDR, matnum);    
}  
//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小
//mode:0,正常显示,1,叠加显示	 
//正点原子@HYW
//CHECK:09/10/30
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 x0=x;
	u8 matnum=(size*size)>>3;
    u8 dzk[72];

	//设置窗口
	LCD_SetDisplayWindow(x, y, x+size-1, y+size-1);

	Get_HzMat(font,dzk,size);//得到相应大小的点阵数据

	if((mode&0x01)==0)//正常显示
	{	 
	    for(t=0;t<matnum;t++)
	    {   												   
		    temp=dzk[t];//得到12数据                          
	        for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)
				LCD_WriteData(POINT_COLOR);
	 			else 
				LCD_WriteData(BACK_COLOR);
				temp<<=1;
			}  	 
    	} 
	}else//叠加显示
	{
	    for(t=0;t<matnum;t++)
	    {   												   
		    temp=dzk[t];//得到12数据                          
	        for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)
				LCD_DrawPoint(x,y);   
				temp<<=1;
				x++;
				if((x-x0)==size)
				{
					x=x0;
					y++;
				}
			}  	 
    	} 
	}   
}
//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//str  :字符串
//size :字体大小
//mode:bit0:0,非叠加方式;1,叠加方式  
//     bit1:1,非自动换行;1,自动换行    
//正点原子@HYW
//CHECK:09/10/30			   
void Show_Str(u16 x,u16 y,u8*str,u8 size,u8 mode)
{												  	  
    u8 bHz=0;     //字符或者中文  	    				    				  	  
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {	
	        if(*str>=StartSec)
			bHz=1;//中文 
	        else              //字符
	        {      
                if(x>(LCD_W-size/2))//换行
				{				   
					if(mode&0x02)return;//判断是不是要自动换行
					else
					{
						y+=size;
						x=0;
					}		   
				}							    
		        if(y>(LCD_H-size))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					x=0;
		            str++; 
		        }  
		        else 
				LCD_DisplayChar(x,y,*str,size,mode);//有效部分写入 
				str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
        }else//中文 
        {     
            bHz=0;//有汉字库    
            if(x>(LCD_W-size))//换行
			{	    
				if(mode&0x02)return;//判断是不是要自动换行
				else
				{
					y+=size;
					x=0;
				}		  
			}
	        if(y>(LCD_H-size))break;//越界返回  						     
	        Show_Font(x,y,str,size,mode); //显示这个汉字,空心显示 
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}  			 		  
//得到字符串的长度
//正点原子@HYW
//CHECK OK 091118 
u16 my_strlen(u8*str)
{
	u16 len=0;
	while(*str!='\0')
	{
		str++;
		len++;
	}
	return len;
}
//在str1后面加入str2
//正点原子@HYW
//CHECK OK 091118 
void my_stradd(u8*str1,u8*str2)
{				  
	while(*str1!='\0')str1++;
	while(*str2!='\0')
	{
		*str1=*str2;
		str2++;
		str1++;
	}
	*str1='\0';//加入结束符  		 
}  
//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度
//针对12*12字体!!!
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u16 len)
{
	u16 strlenth=0;
   	strlenth=my_strlen(str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str(strlenth+x,y,str,size,1);
	}
}   

























		  






