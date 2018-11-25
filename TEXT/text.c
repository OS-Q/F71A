#include "lcd.h"
#include "text.h"
#include "fontupd.h"
#include "flash.h"	  
//Mini STM32������
//�ı���ʾ���� V1.1					  
//����ԭ��@ALIENTEK
//2010/5/23		

#define NumWordPerSec 94
#define StartSec 0xA1
#define StartIndex 0xA1
#define EndSec 0xF7
#define EndIndex 0xFE 

		   
//////////////////////////////////////////////���ú���////////////////////////////////////////                      
//code �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַ,ascii��
//mat  ���ݴ�ŵ�ַ size*2 bytes��С
//����ԭ��@HYW
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
	if(qh<StartSec||ql<StartIndex||ql>EndIndex||qh>EndSec)//�� ���ú���
	{   		    
	    for(i=0;i<matnum;i++)*mat++=0x00;//�������
	    return; //��������
	}          

	foffset=((qh-StartSec)*NumWordPerSec+(ql-StartIndex))*matnum;//�õ��ֿ��е��ֽ�ƫ����  		  

	if(size==16) SPI_Flash_Read(mat, foffset+FONT16ADDR, matnum);
	else if(size==12) SPI_Flash_Read(mat, foffset+FONT12ADDR, matnum);
	else SPI_Flash_Read(mat, foffset+FONT24ADDR, matnum);    
}  
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
//mode:0,������ʾ,1,������ʾ	 
//����ԭ��@HYW
//CHECK:09/10/30
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 x0=x;
	u8 matnum=(size*size)>>3;
    u8 dzk[72];

	//���ô���
	LCD_SetDisplayWindow(x, y, x+size-1, y+size-1);

	Get_HzMat(font,dzk,size);//�õ���Ӧ��С�ĵ�������

	if((mode&0x01)==0)//������ʾ
	{	 
	    for(t=0;t<matnum;t++)
	    {   												   
		    temp=dzk[t];//�õ�12����                          
	        for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)
				LCD_WriteData(POINT_COLOR);
	 			else 
				LCD_WriteData(BACK_COLOR);
				temp<<=1;
			}  	 
    	} 
	}else//������ʾ
	{
	    for(t=0;t<matnum;t++)
	    {   												   
		    temp=dzk[t];//�õ�12����                          
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
//��ָ��λ�ÿ�ʼ��ʾһ���ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//str  :�ַ���
//size :�����С
//mode:bit0:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ  
//     bit1:1,���Զ�����;1,�Զ�����    
//����ԭ��@HYW
//CHECK:09/10/30			   
void Show_Str(u16 x,u16 y,u8*str,u8 size,u8 mode)
{												  	  
    u8 bHz=0;     //�ַ���������  	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {	
	        if(*str>=StartSec)
			bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(LCD_W-size/2))//����
				{				   
					if(mode&0x02)return;//�ж��ǲ���Ҫ�Զ�����
					else
					{
						y+=size;
						x=0;
					}		   
				}							    
		        if(y>(LCD_H-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=0;
		            str++; 
		        }  
		        else 
				LCD_DisplayChar(x,y,*str,size,mode);//��Ч����д�� 
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(LCD_W-size))//����
			{	    
				if(mode&0x02)return;//�ж��ǲ���Ҫ�Զ�����
				else
				{
					y+=size;
					x=0;
				}		  
			}
	        if(y>(LCD_H-size))break;//Խ�緵��  						     
	        Show_Font(x,y,str,size,mode); //��ʾ�������,������ʾ 
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}  			 		  
//�õ��ַ����ĳ���
//����ԭ��@HYW
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
//��str1�������str2
//����ԭ��@HYW
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
	*str1='\0';//���������  		 
}  
//��ָ����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ��
//���12*12����!!!
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

























		  






