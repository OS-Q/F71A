#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "fat.h"
#include "sysfile.h"
#include "jpegbmp.h"
#include "usart.h"
#include "ebookplay.h"	 
	    
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK
						 
//文本浏览器(.txt/.c/.h/.lrc)	  
//支持FAT32/16文件系统的浏览.  
void Ebook_Play(void)
{				 
	FileInfoStruct *FileInfo;		 			 
	u16 index=0;  //当前选择的文件编号	   
	//从根目录开始浏览
	if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//根目录簇号
 	else Cur_Dir_Cluster=0;						   
	Fat_Dir_Cluster=Cur_Dir_Cluster;//父目录就是当前目录
	while(1)
	{	  
BROS:
		Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发
		POINT_COLOR=WHITE;	   
		Load_Gui_Bro("选择");//显示GUI界面
		Show_Str(128,2,"电子图书",16,0x01);//叠加模式,非自动换行
			
		index=File_Browse(T_LRC|T_TXT|T_C|T_H);//浏览文件夹    
		if(!index)return;//结束浏览,退出此功能!!!!!	
				 												   
		Pen_Point.Key_Sta=Key_Up;         //释放显示期间的触发 	   
		FileInfo=&F_Info[0];//开辟暂存空间.	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_LRC|T_TXT|T_C|T_H,&index);//得到这张图片的信息	  
		Read_Book(FileInfo);//读取TXT文件 		 	   
		goto BROS;  	  
	}
}
#define FSIZE 16
//文本文件读取程序	 
//实现功能:把文件 FileName 打开.在液晶上显示出来 
//打开txt/lrc文件  							 
void Read_Book(FileInfoStruct *FileName)
{     							   
    u16 pointpos=0;   //指针位置   
    unsigned long offlenth=0;//文件读取的大小 		   
    s16 x=0,y=0;
    u8 *p=0;            //指向txt文件
    u8 temp[2]={0,0};   //换扇区使用 
    u8 bHz=0;           //字符或者中文  	  
 				  
	u8 key,keyx,tcnt=0;
	u8 enout=0;//能否退出的标志 
	u8 keycnt=0;

  
 	if((FileName->F_Type&(T_LRC|T_TXT|T_C|T_H))==0)return;//不是TXT类型的文件
	F_Open(FileName);//打开文件	  

	LCD_Clear(WHITE);//清屏
	BACK_COLOR=WHITE;
	POINT_COLOR=DARKBLUE;//深蓝色字体    
	while(1)
    {  	   	    	
		F_Read(FileName,jpg_buffer);//读取数据	 			 
	 	p=jpg_buffer; //指向txt数据首地址   
		do
        {   				    
	        while(*p!=0)//数据未结束
	        { 						  
		        if(!bHz)
		        {
			        if(*p>0x80)bHz=1;//中文字符 
			        else             //ASCII字符
			        {      
                        if(x>(320-FSIZE/2)){y+=FSIZE;x=0;}//一行已满,换行  
				        if(y>(480-FSIZE))break;//越界返回      
				        if(*p==13)//换行符号
				        {         
				            y+=FSIZE;
							x=-FSIZE/2;
				            p++;pointpos++;//跳过 
				        }
						else if(*p==9)
						x+=FSIZE*2-FSIZE/2; 
				        else LCD_DisplayChar(x,y,*p,FSIZE,0); 
						p++;pointpos++; 
				        x+=FSIZE/2; //字符,为全字的一半 
			        }
		        }else
		        {     
		            bHz=0;//清除汉字标志 
		            if(x>(320-FSIZE)){y+=FSIZE;x=0;}//换行
			        if(y>(480-FSIZE))break;//越界返回  
			        //临界处理   
			        if(pointpos==511)//保存上一扇区的最后一个字节,退出        
			        {
			            temp[0]=*p; //记录最后的字节                
			            break;      //此扇区数据已经读完
			        }
			        if(temp[0])//上一次没有显示完
			        {                         
			            temp[1]=*p;//取第二扇区的第一个字节
			            
						Show_Font(x,y,temp,16,0); //显示这个汉字,空心显示	
			            p++;pointpos++;//偏移一个字节
			            temp[0]=0;     //清除标记
			        }else Show_Font(x,y,p,16,0); //显示这个汉字,空心显示			   
			        
			        p+=2;pointpos+=2;     //指针偏移 
			        x+=FSIZE; 			  //下一个汉字偏移
		        }
		        if(pointpos>511)break;
	        }  
            if(pointpos>=511)break;//一个扇区读取完毕,直接跳出,读下一个扇区,防止停顿		   
 			enout=0; //退出使能标志不使能
			key=0;		 
			while(1) //是否显示下一屏数据
            {	    
				keyx=AI_Read_TP(30,10);	    
				if(keyx)//按键按下,并且支持点连按 
				{	  		  
					if(enout&&Is_In_Area(280,450,40,30)) return;//返回
					 
					key=Touch_To_Num(7);	
 				 	if(key==KEY_NEXT||key==KEY_PREV)//向上/向下按的时候,要先确定是否松开了
					{
						if(Pen_Point.Key_Reg&0x80)key=0;//没经过松开	 
					}		 											    
					Pen_Point.Key_Reg|=0x80;//标记按下
				}else 
				{
					Pen_Point.Key_Reg&=~0x80;//标记松开
					keycnt=0;
				}   	 
					   
				if(key==KEY_NEXT)//向下翻页
                {     			   
 					keycnt=0;	  
 					if(y>(480-FSIZE)){x=0;y=0;}//非结尾返回,屏幕已满,清屏       
                    if(offlenth+pointpos+1>=FileName->F_Size)goto QUIT;//文件读取结束(长度结束) 	    
                    else 
					{
						LCD_Clear(WHITE);//清屏
						break;
			 		} 
                }
				else if(key==KEY_FUNC)//按中中间区域了							   
				{									   
 					keycnt++;
					if(keycnt>=200)
					{ 
QUIT:
						keycnt=0;
						enout=1;//使能退出		    	 
						LCD_Fill(0,458,319,458,DARKBLUE);
						LCD_Fill(0,459,319,468,LIGHTBLUE);//填充底部颜色(浅蓝色)	
						LCD_Fill(0,469,319,479,GRAYBLUE); //填充底部颜色(灰蓝色)	  	   
						POINT_COLOR=WHITE;//白色字体
						Show_Str(284,460,"返回",16,0x01);    //叠加模式,非自动恍 
						POINT_COLOR=DARKBLUE;//深蓝色
					}  
				}
				else{}
				key=0;//清除此次按键	   			  					  
                Delay_ms(10); 
				tcnt++;
				if(tcnt>=20)
				{
					SignalLED=!SignalLED;
					tcnt=0;
				}
            }   
        }while(pointpos<511);  
        offlenth+=pointpos+1;//文件长度累加     
		pointpos=0;//清除指针计数器 		    		  	 
    } 
}  






