#include "picviewer.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "fat.h"
#include "sysfile.h"
#include "jpegbmp.h"
#include "usart.h"	
#include "sys.h"
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK
 
//图片浏览器
//支持FAT32/16文件系统的浏览.	  
void Pic_Viewer(void)
{		    
	u8 key,temp;
	FileInfoStruct *FileInfo;		 
	u16 pic_cnt=0;//当前目录下图片文件的个数
	u16 index=0;  //当前选择的文件编号	
	u8 enout=0; 
	u8 keycnt=0; 
				    
	u16 tcnt=0;	
	u8 pause=0;//暂停与否标志	    
	//从根目录开始浏览
	if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//根目录簇号
 	else Cur_Dir_Cluster=0;						   
	Fat_Dir_Cluster=Cur_Dir_Cluster;//父目录就是当前目录    			   	  
	while(1)
	{
BROS:	 							  			 
		Pen_Point.Key_Sta=Key_Up;
		Load_Gui_Bro("选择");//显示GUI界面	
		POINT_COLOR=WHITE;	 
		Show_Str(128,2,"数码相框",16,0x01);//叠加模式,非自动换行
		index=File_Browse(T_JPG|T_JPEG|T_BMP);//浏览文件夹    
		if(!index)return;//结束浏览,退出此功能!!!!!

		pic_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPG|T_JPG|T_BMP,&pic_cnt);//获取当前文件夹下面的目标文件个数 
		Pen_Point.Key_Sta=Key_Up;         //释放显示期间的触发 
		FileInfo=&F_Info[0];//开辟暂存空间.
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPG|T_JPG|T_BMP,&index);//得到这张图片的信息	 
			LCD_Clear(WHITE);//清屏,加载下一幅图片的时候,一定清屏   
			AI_LoadPicFile(FileInfo,0,0,319,479);//显示图片	   
			POINT_COLOR=RED;
			Show_Str(0,0,FileInfo->F_Name,16,0x03);//自动换行,叠加
			Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发 
			key=0;enout=0;
			tcnt=0; 	 
			while(1)
			{
	 			temp=AI_Read_TP(30,1);	    
				if(temp)//按键按下,并且支持点连按 
				{	  		  
					if(enout&&Is_In_Area(280,440,40,40))
					goto BROS;	  //返回 

					key=Touch_To_Num(7);	
 				 	if(key==KEY_NEXT||key==KEY_PREV)//向上/向下按的时候,要先确定是否松开了
					{
						if(Pen_Point.Key_Reg&0x80)key=0;//没经过松开	 
					}		 											    
					if((key==KEY_FUNC)&&(keycnt==0)&&((Pen_Point.Key_Reg&0x80)==0))pause=!pause;//单次按下,则暂停取反	 
					Pen_Point.Key_Reg|=0x80;//标记按下
				}else 
				{
					Pen_Point.Key_Reg&=~0x80;//标记松开
					keycnt=0;
				}   	 			  		
				if(key)
				{								 
 					switch(key)
					{
						case KEY_PREV:	  
							enout=0;
							if(index>1)index--;
							else index=pic_cnt;//到最后一副(循环) 
							break;	
						case KEY_NEXT:
							enout=0; 
		   					index++;
							if(index>pic_cnt)index=1;//显示第一副,循环
							break;
						case KEY_FUNC://短按暂停，长按退出
							keycnt++;
							tcnt=0;//清空时间计数器	    
							if(keycnt>=20)
							{ 				 
								enout=1;//使能退出
								keycnt=0;			  
							 	POINT_COLOR=DARKBLUE;//深蓝色	 
								LCD_DrawLine(0,458,319,458);
							 	LCD_Fill(0,459,319,468,LIGHTBLUE);//填充底部颜色(浅蓝色)	
							 	LCD_Fill(0,469,319,479,GRAYBLUE); //填充底部颜色(灰蓝色)	  	   
								POINT_COLOR=WHITE;//白色字体
								Show_Str(284,460,"返回",16,0x01);    //叠加模式,非自动恍  
							} 	
					}
					if(key!=KEY_FUNC)break;//上下翻页		  
					key=0;//清除键值		
					Delay_ms(100); 
				}   
				Delay_ms(1);  
				if(tcnt>=3000 && pause==0)//延迟时间到了
				{	 
					tcnt=0;
   					index++;
					if(index>pic_cnt)index=1;//显示第一副,循环 
					break; 
				}else if(tcnt%200==0)
				{
					if(pause==0)SignalLED=!SignalLED;//闪烁,证明是自动播放
					else SignalLED=0;//常亮,证明是暂停
				}
				tcnt++;//时间累加			 
			}
		}
	}			 
}




