#include "untogb.h"
#include "un_gb_tab.h"
#include "fontupd.h"
#include "flash.h"
//#include "usart.h"
//#include "fontupd.h"    	 
//Mini STM32������
//UNICODE TO GBK ����ת������ V1.1
//����ԭ��@ALIENTEK
//2010/5/23			

//��UNICODE��ת��ΪGBK��
//unicode:UNICODE��
//����ֵ:GBK��				 
u16 UnicodeToGBK(u16 unicode)//�ö��ֲ����㷨
{
   	s32 i,j,k;
	u32 offset;
	u16 temp;
	i = 0;
	j = 682;

	if((unicode >= 0x4E00) && (unicode <= 0x9FA5))		//�Ǻ���
	{
		offset = ((unicode - 0x4E00)<<1) + UNI2GBKADDR;
		SPI_Flash_Read((u8 *)(&temp),offset,2);//�õ�GBK��
		return temp;
	}
	else									//������
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
	return '?';    //δ�ҵ��ı��룬���أ�
}						 							  
//��pbuf�ڵ�unicode��תΪgbk��.
//pbuf:unicode��洢��,ͬʱҲ��gbk��������.����С��80���ֽ�.
//����ת��unit code-> GBK
//����ԭ��@HYW
//CHECK:09/10/30
void UniToGB(u8 *pbuf)
{   					  
	unsigned int  code;
	unsigned char i,m=0; 
	for(i=0;i<80;i++)//�80���ַ�
	{	  
		code = pbuf[i*2+1]*256 + pbuf[i*2]; 
		if((code==0)||(code==0xffff))break;

		if(code<0x20)			//�����ַ�
		pbuf[m++] = '?';

		else if(code<=0x7e)		//ASCII�ַ�			
		pbuf[m++]=(unsigned char)code;
		              
		else		//GBK����
		{   								    
			code=UnicodeToGBK(code);//��unicodeת��ΪGBK
			if((code&0xFF00)!=0) 	 //����ֵΪ��ʱ��˵��GBK���޶�Ӧ����  
			{
				pbuf[m++]=(u8)code;	 	 //δ���庺�֣���ʾ����������8λ
				pbuf[m++]=code>>8; 
			}
			else pbuf[m++]=(u8)code;
		}						 
	}    
	pbuf[m]='\0';//��ӽ�������  			      
}
