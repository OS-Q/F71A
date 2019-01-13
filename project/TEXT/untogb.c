#include "untogb.h"
#include "un_gb_tab.h"
#include "fontupd.h"
#include "flash.h"
//#include "usart.h"
//#include "fontupd.h"    	 
//Mini STM32开发板
//UNICODE TO GBK 内码转换程序 V1.1
//正点原子@ALIENTEK
//2010/5/23			

//将UNICODE码转换为GBK码
//unicode:UNICODE码
//返回值:GBK码				 
u16 UnicodeToGBK(u16 unicode)//用二分查找算法
{
   	s32 i,j,k;
	u32 offset;
	u16 temp;
	i = 0;
	j = 682;

	if((unicode >= 0x4E00) && (unicode <= 0x9FA5))		//是汉字
	{
		offset = ((unicode - 0x4E00)<<1) + UNI2GBKADDR;
		SPI_Flash_Read((u8 *)(&temp),offset,2);//得到GBK码
		return temp;
	}
	else									//标点符号
	{
		while(i >= j)
		{
			k = (i+j)>>1;
			if(unicode < UnicodeToGB2312_Tab[k][0])
			j = k-1;
			else if(unicode > UnicodeToGB2312_Tab[k][0])
			i = k+1;
			else
			return UnicodeToGB2312_Tab[k][1];
		}
	}			 
	return '?';    //未找到的编码，返回？
}						 							  
//将pbuf内的unicode码转为gbk码.
//pbuf:unicode码存储区,同时也是gbk码的输出区.必须小于80个字节.
//代码转换unit code-> GBK
//正点原子@HYW
//CHECK:09/10/30
void UniToGB(u8 *pbuf)
{   					  
	unsigned int  code;
	unsigned char i,m=0; 
	for(i=0;i<80;i++)//最长80个字符
	{	  
		code = pbuf[i*2+1]*256 + pbuf[i*2]; 
		if((code==0)||(code==0xffff))break;

		if(code<0x20)			//控制字符
		pbuf[m++] = '?';

		else if(code<=0x7e)		//ASCII字符			
		pbuf[m++]=(unsigned char)code;
		              
		else		//GBK汉字
		{   								    
			code=UnicodeToGBK(code);//把unicode转换为GBK
			if((code&0xFF00)!=0) 	 //返回值为？时，说明GBK中无对应汉字  
			{
				pbuf[m++]=(u8)code;	 	 //未定义汉字，显示？，舍弃高8位
				pbuf[m++]=code>>8; 
			}
			else pbuf[m++]=(u8)code;
		}						 
	}    
	pbuf[m]='\0';//添加结束符号  			      
}
