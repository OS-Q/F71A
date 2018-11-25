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

//系统主菜单
const u8 *Menu[12]=
{
"音乐播放",			
"数码相框",
"电子图书",		  
" 照相机 ",
"拼图游戏",		 
"USB鼠标",
"USB传输 ",
"无线传输",	
"时钟显示",
"文件浏览",
"触摸画板",
"系统设定"	 
};
//////////////////////////GUI 按钮///////////////////////////////
void GUI_DrawLine(u16 x,u16 y,u16 xlen,u16 ylen,u16 color)
{
	u16 tempcolor=POINT_COLOR;
	POINT_COLOR=color;
	LCD_DrawLine(x,y,x+xlen,y+ylen);
	POINT_COLOR=tempcolor;	
}
//在指定位置显示指定颜色的按钮
//(x,y):按钮起始坐标
//xlen,ylen:x,y方向的长度
//str:按钮上要显示的字符
//color:字符颜色
void GUI_Draw_Button(u16 x,u16 y,u16 xlen,u16 ylen,u8 *str,u16 color)
{	
	u16 tback_color,tpoint_color;
	//////////////////画按钮部分//////////////////////   
	LCD_Fill(x,y,x+xlen,y+ylen,LGRAY);//画实体
	GUI_DrawLine(x,y,xlen,0,WHITE);   //上外边框
	GUI_DrawLine(x,y,0,ylen,WHITE);   //左外边框   	   
	GUI_DrawLine(x+1,y+1,xlen-1,0,0XB5B6);//上内边框
	GUI_DrawLine(x+1,y+1,0,ylen-1,0XB5B6);//左内边框
	GUI_DrawLine(x,y+ylen,xlen,0,0X8431); //下外边框
	GUI_DrawLine(x+xlen,y,0,ylen,0X8431); //右外边框   	   
	GUI_DrawLine(x+1,y+ylen-1,xlen-2,0,0XA535);//下内边框
	GUI_DrawLine(x+xlen-1,y,0,ylen-1,0XA535);  //右内边框
	//////////////////显示按钮上面的信息///////////////  
	if(ylen<16)return;//不够画	 
	tback_color=BACK_COLOR;
	tpoint_color=POINT_COLOR;
	BACK_COLOR=LGRAY;
	POINT_COLOR=color; 
	Show_Str_Mid(x,y+(ylen-16)/2,str,16,xlen);//只对16字体
	BACK_COLOR=tback_color;	 //恢复之前的颜色
	POINT_COLOR=tpoint_color; 
}
/////////////////////////界面图形加载/////////////////////////////

//选择菜单
void Select_Menu(u8 M_Type,u8 Cnt,u8 En)
{
	u16 tx,ty;
	u8 t;	 
	if(M_Type==0)//主菜单
	{	  
		tx=100*(Cnt%3)+30;//10,90,170			
		ty=110*(Cnt/3)+20;	
		//显示外框部分 
		if(En)POINT_COLOR=BLUE;
		else POINT_COLOR=WHITE;
		for(t=0;t<4;t++)
		{
			LCD_DrawRect(tx-10+t,ty-10+t,tx+70-t,ty+70-t); 
		} 
		//显示字体，反白
		if(En)//选中当前的菜单
		{	  
			POINT_COLOR=WHITE;//白色字体
			BACK_COLOR=LBBLUE;  //蓝色背景      
		}else
		{
			POINT_COLOR=DARKBLUE;//深蓝色字体
			BACK_COLOR=WHITE; //白色背景   
		}	 												  
		Show_Str(tx-2,ty+74,(u8*)Menu[Cnt],16,0);	
	}
}
//加载主界面
void Load_Sys_ICO(void)
{		  
	u8 t;
	u16 ty,tx;	   
	FileInfoStruct temp; 
	POINT_COLOR=RED;//黑色字体				   
    LCD_Clear(WHITE);//清屏 
	temp.F_Type=T_BMP;//文件属性设置 
	for(t=0;t<12;t++)
	{	   
		temp.F_StartCluster=sys_ico[t];								    
		tx=100*(t%3)+30;//25,115,205	
		ty=110*(t/3)+20;    
		AI_LoadPicFile(&temp,tx,ty,tx+59,ty+59);
		Select_Menu(0,t,0);//加载字体    		    
	}
}
//加载游戏界面
//size:3,3*3图片
//     4,4*4图片
//     5,5*5图片
//#define FRAME1 0XBC40 //棕色
void Load_Game_ICO(u8 size)
{
	u8 i;
	LCD_Clear(LGRAY);//清屏
	for(i=0;i<5;i++)//画大边框
	{		
		POINT_COLOR=FRAME1;
		LCD_DrawRect(i,i,319-i,319-i);
	}
	if(size==3)
	{
		for(i=0;i<5;i++)//画小边框
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRect(206+i,315+i,319-i,422-i);
		}									 
	    LCD_Fill(211,315,314,319,LGRAY);//把多余的去掉	
		POINT_COLOR=RED;
		LCD_DrawRect(211,314,314,417);//画空的方框，事先不放图片
		LCD_DrawRect(5,5,314,314); //画最大的红框

		LCD_DrawLine(108,6,108,313);  //画内部红框 竖线
		LCD_DrawLine(211,6,211,313);//画内部红框
		LCD_DrawLine(6,108,313,108);  //画内部红框 横线
		LCD_DrawLine(6,211,313,211);//画内部红框
	}else if(size==4)
	{
		for(i=0;i<4;i++)//画小边框
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRect(234+i,316+i,319-i,398-i);
		}									 
	    LCD_Fill(238,316,314,319,LGRAY);//把多余的去掉	
		POINT_COLOR=RED;
		LCD_DrawRect(238,316,316,394);//画空的方框，第十六格
		LCD_DrawRect(4,4,316,316);    //画最大的红框

		for(i=1;i<size;i++)
		LCD_DrawLine(5,4+78*i,315,4+78*i);  //画内部横网格线
		for(i=1;i<size;i++)
		LCD_DrawLine(4+78*i,5,4+78*i,315);  //画内部竖网格线	   										  
	}else if(size==5)
	{
		for(i=0;i<5;i++)//画小边框
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRect(247+i,315+i,319-i,382-i);
		}										 
	    LCD_Fill(252,315,314,319,LGRAY);//把多余的去掉	
		POINT_COLOR=RED;
		LCD_DrawRect(252,314,314,377);//画空的方框，第十六格
		LCD_DrawRect(4,4,314,314);    //画最大的红框

		for(i=1;i<size;i++)
		LCD_DrawLine(5,4+62*i,313,4+62*i);  //画内部横网格线
		for(i=1;i<size;i++)
		LCD_DrawLine(4+62*i,5,4+62*i,313);  //画内部竖网格线
	}	  		  								    	  		  
	LCD_DrawRect(2,340,105,443);//包括预览图片
	
	BACK_COLOR=WHITE;
	POINT_COLOR=BLUE;//画笔颜色
	Show_Str(110,350,"等级:",16,0x01);
	POINT_COLOR=RED;//画笔颜色
	if(size==3)Show_Str(160,350,"简单",16,0x01);
	else if(size==4)Show_Str(160,350,"一般",16,0x01);
	else Show_Str(160,350,"困难",16,0x01);
	POINT_COLOR=BLUE;//画笔颜色
	Show_Str(110,370,"步数:",16,0x01);
	Show_Str(110,390,"用时:",16,0x01);   
	GUI_Draw_Button(130,420,40,24,"退出",BLACK);		  					     
}

void Load_Draw_Dialog(void)
{
	if(PEN == 0)
	{
		LCD_SetBackColor(Blue1);
		LCD_SetTextColor(Blue);//设置字体为蓝色
		LCD_DisplayStringLine(Line0, 37, "RST");
		Delay_ms(200);
	}
	LCD_Clear(White);//清屏 
	LCD_SetBackColor(White);  
	LCD_DisplayStringLine(Line0, 37, "RST");//显示清屏区域
	LCD_DisplayStringLine(Line0, 0, "EXIT");
  	LCD_SetTextColor(Red);//设置画笔蓝色 
}
 		 	 
//加载浏览界面
//设置界面 
//str:"选择/确定"
void Load_Gui_Bro(u8 * str)
{			 	   					  			   
    LCD_Clear(WHITE);//清屏
	//顶部 						   
 	LCD_Fill(0,0,319,9,LIGHTBLUE);  //填充底部颜色(浅蓝色)	
 	LCD_Fill(0,10,319,20,GRAYBLUE); //填充底部颜色(灰蓝色)
	//中部
	POINT_COLOR=DARKBLUE;//深蓝色  
	LCD_DrawLine(0,21,319,21);
	LCD_Fill(0,22,319,41,LIGHTBLUE);//填充中部部颜色(浅灰蓝色)
	LCD_DrawLine(0,41,319,41);	    
	//底部
	LCD_DrawLine(0,458,319,458);
 	LCD_Fill(0,459,319,468,LIGHTBLUE);//填充底部颜色(浅蓝色)	
 	LCD_Fill(0,469,319,479,GRAYBLUE);//填充底部颜色(灰蓝色)

	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"请选择:",16,0x01); //叠加模式,非自动换行
	POINT_COLOR=WHITE;
	Show_Str(5,460,str,16,0x01);   //叠加模式,非自动换行
	Show_Str(283,460,"返回",16,0x01); //叠加模式,非自动换行  
}
//加载显示界面
//在PS2/触控画图时用到
//设置界面 
//str:"选择/确定"
void Load_Gui_Show(u8 *lbtstr,u8 *mbtstr,u8 *rbtstr)
{			 	   					  			   
    LCD_Clear(WHITE);  //清屏  
 	POINT_COLOR=DARKBLUE;//深蓝色	 
	LCD_DrawLine(0,457,319,457);
 	LCD_Fill(0,458,319,467,LIGHTBLUE);//填充底部颜色(浅蓝色)	
 	LCD_Fill(0,468,319,479,GRAYBLUE); //填充底部颜色(灰蓝色)	  	   
	POINT_COLOR=WHITE;//白色字体
	Show_Str(5,460,lbtstr,16,0x01);    //叠加模式,非自动换行
	Show_Str(144,460,mbtstr,16,0x01);  //叠加模式,非自动换行		  
	Show_Str(283,460,rbtstr,16,0x01);  //叠加模式,非自动换行  
}

////////////////////////////////与触摸屏控制相关的函数////////////////////////////
//把触摸屏得到的数据转换为数字(按钮值)
//M_Type:按钮模式;
//0,系统菜单	
//1,文件浏览模式 	
//2,时间模式下,按键的获取,连按支持! 
//3,音乐播放模式下
//4,功能选择模式下
//5,收音机模式下
//6,闹钟中断程序按键

//7,TXT浏览/JPEG浏览 时按键处理			  
//返回值:对应的按键值(1~n)
//返回0,则表示此次按键无效  
u8 Touch_To_Num(u8 M_Type)
{
	u16 xtemp;
	u16 t,ytemp;
	switch(M_Type)
	{
		case 0://系统菜单下										  
			for(t=0;t<12;t++)
			{   
				xtemp=100*(t%3)+30;
				ytemp=110*(t/3)+20;
				if(Is_In_Area(xtemp,ytemp,60,90))return t+1;//返回按键值 
			}
			return 0;			  
		case 1://1,文件浏览模式	   	  
			for(t=0;t<13;t++)
			{	  
				if(Is_In_Area(0,42+t*32,160,32))return t+1;
			}
			if(Is_In_Area(0,450,40,30))return 14;//选择按钮被按下
			if(Is_In_Area(280,450,40,30))return 15;//返回按钮被按下
			return 0;//按键无效	    
//			if(xtemp)return xtemp&0xf0;//滑动触摸了!! 
			//break; 
		case 2://4,功能选择模式下 
			for(t=0;t<13;t++)
			{	  
				if(Is_In_Area(0,42+t*32,320,32))return t+1;
			}
			if(Is_In_Area(0,450,40,30))return 14;//选择按钮被按下
			if(Is_In_Area(280,450,40,30))return 15;//返回按钮被按下
			return 0;//按键无效	 
			//break;

		case 3:
			if(Is_In_Area(280,270,30,15))return 1;//循环模式更改	    
			else if(Is_In_Area(200,270,30,15))return 2; //EQ
			else if(Is_In_Area(240,270,30,15))return 3;   //LRC
			else if(Is_In_Area(VOLBAR_X,VOLBAR_Y-12,78,13))return 4;//音量
			else if(Is_In_Area(290,5,20,20))return 5;//关闭	 
				
			else if(Is_In_Area(20,240,30,30))return 6;//上一取
			else if(Is_In_Area(75,230,50,50))return 7;//播放
			else if(Is_In_Area(150,240,30,30))return 8;//下一曲

			else if(Is_In_Area(20,195,280,10))return 9;//进度条按下,y方向扩大了一点范围

			else
			return 0;//按键无效
		case 4:						    
			break;
		case 5://5,收音机模式下 
			break; 
		case 6://6,闹钟中断程序按键	  
			break;
		case 7://JPEG/BMP/TXT浏览按键模式  
			if(Is_In_Area(0,0,319,120))return KEY_PREV;        //向上
			else if(Is_In_Area(0,120,319,240))return KEY_FUNC; //不定		    
			else if(Is_In_Area(0,360,319,120))return KEY_NEXT; //向下翻 	   
			return 0;//按键无效	    			
			//break; 	  	    		 
	}
	return 0;
}
//按键滑动检测
u8 KEY_Move_Check(void)
{
#define MOVE_PIXES 50 //滑动距离						    
	if(Pen_Point.X>Pen_Point.X0)//右移
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_RIGHT;
	}else  //左移
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_LEFT;
	}
	if(Pen_Point.Y>Pen_Point.Y0)//下移
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_DOWN;
	}else			   //上移
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_UP;
	}	 
	return 0;							 
}
//////////////////////////////与文件浏览相关的函数//////////////////////////////
//功能选择调度函数	    
//list  ：功能条目指针
//title ：标题
//func  ：选择/确定/。。。
//len   ：功能条目的个数(1~8)
//sel   ：开始选中的条目(0~7)
//返回值：0~7,选中的条目；8，没有选择任何条目。
u8 Load_Fun_Sel(const u8 * *list,const u8 *title,u8 *func,u8 len,u8 sel)
{	
	u8 key;
	u16 t;
	Load_Gui_Bro(func);	   
	//显示标题
	POINT_COLOR=WHITE;
	Show_Str_Mid(0,2,(u8*)title,16,320); //叠加模式,非自动换行  
	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"请选择:",16,0x01);    //叠加模式,非自动换行	 
	//加载条目信息
	for(t=0;t<len;t++)
	{	 											    
		if(t==sel)
		{
			LCD_Fill(0,45+sel*32,319,37+(sel+1)*32,LBBLUE );//对选中的条目，填充棕色 
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;	  
		Show_Str(0,50+t*32,(u8*)list[t],16,0x01); //叠加模式,非自动换行       
	} 
 	key=0;
	Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发	    
	while(1)//等待按键
	{	 
		key=AI_Read_TP(30,1);
		if(key)Pen_Point.Key_Reg|=0X80;//标记按键已经按下了 	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//按键松开了
		{
			Pen_Point.Key_Reg=0;			    
			key=Touch_To_Num(2);
			if(key)//有按键按下
			{  									    
				switch(key)
				{
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
					case 14://确认键按下
					key-=1;
					if(sel!=key&&key<len)//选中一个
					{
						POINT_COLOR=DARKBLUE;//画笔颜色选择		 
						LCD_Fill(0,45+sel*32,319,37+(sel+1)*32,WHITE);//清空上次选择的
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//重新显示上次的名字
			        	sel=key;//选择这次选择的
						LCD_Fill(0,45+sel*32,319,37+(sel+1)*32,LBBLUE);//填充底色
						POINT_COLOR=WHITE;//画笔颜色选择		 
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//显示名字 
					}else if(sel==key||key==13)//选择按钮,或者双击
					{	 	
						return sel;//返回选择的条目号
					}
						break;  		  	  
					case 15:return 13;//选择了"返回"按钮,返回一个不可能值  
				}  
				key=0;//取消按键值
			}											    
		}	  
		Delay_ms(1);
		t++;
		if(t%500==0) SignalLED=!SignalLED;	
	}  
}
//显示文件
//fcnt:要显示文件的多少
//folder:文件夹个数
//sel :选中第几个1~8 
//返回值:要打开的文件编号1~8(包括文件夹,在FileInfo里面的)	    
//返回9,代表直接退回上一层目录  		    
//显示文件
//check ok 09/04/19
u8 Show_File(u8 fcnt,u8 folder,u8 sel)
{
	u16 t;	
	u8 key; 					   
	FileInfoStruct temp;
	temp.F_Type=T_BMP;//文件属性设置

	sel-=1;										 
	LCD_Fill(0,42,319,457,WHITE);//填充白色,清空上次的显示
	for(t=0;t<fcnt;t++)
	{
		if(F_Info[t].F_Type==T_FILE)temp.F_StartCluster=file_ico[0];//文件夹图标
		else 
		{
			if(F_Info[t].F_Type&(T_JPG|T_JPEG|T_BMP))temp.F_StartCluster=file_ico[2];//图片类图标
			else if(F_Info[t].F_Type&(T_MP3|T_OGG|T_WMA|T_WAV|T_FLAC|T_MID))temp.F_StartCluster=file_ico[1];//音乐类图标
			else if(F_Info[t].F_Type&(T_LRC|T_TXT|T_C|T_H))temp.F_StartCluster=file_ico[3];//文字类图标
			else temp.F_StartCluster=file_ico[4];//未知文件图标
		}				   
		AI_LoadPicFile(&temp,0,42+t*32,31,42+t*32+31);//加载图标  
		if(t==sel)
		{
			LCD_Fill(32,45+sel*32,319,37+(sel+1)*32,LBBLUE);//填充棕色 BROWN	  
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;
		Show_Str(32,50+t*32,F_Info[t].F_Name,16,0x03); //叠加模式,非自动换行 
		//printf("N[%d]%s\n",t,F_Info[t].F_Name);      
	}
	key=0;
	Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发
	while(1)//等待按键操作
	{
		key=AI_Read_TP(50,1);//适合滑动检测	    
		if(key)
		{
			if((Pen_Point.Key_Reg&0x01)==1)//第一次是滑动按下
			{
				t=KEY_Move_Check();//检查滑动按下
				if(t)
				{
					key=t;  
					goto KEY_D;
				}
			}
			Pen_Point.Key_Reg|=0X80;  //标记按键已经按下
		}	   
		else if(Pen_Point.Key_Reg&0X80)//按键松开了
		{
			key=Touch_To_Num(1);
KEY_D:	    
			Pen_Point.Key_Reg=0;		  	
			if(key)//有按键按下
			{
				switch(key)
				{
					case 1:
					case 2:
			 		case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
						key-=1;
						if(sel!=key&&key<fcnt)//选中一个
						{
							POINT_COLOR=DARKBLUE;//画笔颜色选择		 
							LCD_Fill(32,45+sel*32,319,37+(sel+1)*32,WHITE);//清空上次选择的
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//重新显示上次的名字
				        	sel=key;//选择这次选择的
							LCD_Fill(32,45+sel*32,319,37+(sel+1)*32,LBBLUE);//清空上次选择的
							POINT_COLOR=WHITE;//画笔颜色选择		 
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//显示名字 
						}else if((sel==key)&&(BrowseAll||sel<folder))
						 return sel+1;//再次选择,双击打开.
						break;
					case 14: //选择了"选择"按钮
						return sel+1; 	  
					case 15://选择了"返回"按钮 
						return 14;
					case MOVE_UP:  //翻页操作 
					case MOVE_LEFT: 
				    case MOVE_DOWN:  
					case MOVE_RIGHT: 
						return key;
				}
				key=0;//把按键值取消掉				  
				Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发
			}  
		}
		else{}
		Delay_ms(1);
		t++;
		if(t%500==0) SignalLED=!SignalLED;	
	}
}
u8 BrowseAll = 1; 
//浏览文件夹下面的文件
//file_type:要浏览的文件类型
//返回要打开的目标文件编号 1~n(在总目标文件里面的)
//不包括文件夹!!!
//如果返回0则代表退出浏览
u16 File_Browse(u32 file_type)
{			 
	FileInfoStruct *FileInfo;
	u16 folder_cnt;//文件夹的个数
	u16 des_cnt;   //目标文件的个数		    
	u8 fcnt;	   //显示的文件数目
	u16 temp; 					     									  

	u16 total_file;//总文件数目
	u16 pos_cnt;//当前文件号 1~folder_cnt+des_cnt
	u8 key;	  			  
							   			  
Reset:				    
	POINT_COLOR=BLUE;  	   		   //蓝色笔头使用
	folder_cnt=0;des_cnt=0;fcnt=0;key=0;pos_cnt=0;	 	
	Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&folder_cnt);//得到当前目录下文件夹的个数	    
	Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&des_cnt);//得到当前目录下目标文件的个数	 
									   
	total_file=des_cnt+folder_cnt;//总的文件数目=文件夹数目+目标文件数目
	while(1)
	{
		if(pos_cnt<total_file)//文件标尺,未到达文件末尾
		{	   
			FileInfo=&F_Info[fcnt];
			if(pos_cnt<folder_cnt)//找文件夹
			{
			    temp=pos_cnt+1;			    
				Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&temp);
			}else				   //找目标文件
			{
				temp=pos_cnt-folder_cnt+1; 
				Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&temp);
			}
			fcnt++;   //已存的文件数目增加
			pos_cnt++;//文件位置标志增加
		}	  
		if(fcnt==13||pos_cnt==total_file)//找满八个了,或者文件夹已经找完了
		{  
			key=Show_File(fcnt,folder_cnt,1);//显示此次得到的文件
			switch(key)
			{
				case 1://要打开目标文件
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					key-=1;
					if(key<fcnt)//按键在有效范围内
					{
						if(F_Info[key].F_Type==T_FILE)//要打开的是个文件夹
						{
							Cur_Dir_Cluster=F_Info[key].F_StartCluster;//打开这个文件夹		  
							goto Reset;
						}else    //要打开的是目标文件
						{	
							if(BrowseAll)		   
							return pos_cnt+key-folder_cnt-fcnt+1;//返回目标文件编号
						}
					}
					break;
				case 14:	//选择了返回按钮.退出到上一层目录
					//在主目录浏览里面,按下返回按钮,则退出浏览模式	
					if((FAT32_Enable&&Cur_Dir_Cluster==FirstDirClust)||Cur_Dir_Cluster==0)return 0;//退出浏览模式	  					
					Cur_Dir_Cluster=Fat_Dir_Cluster;//退到父目录  			    	 
					goto Reset;	   	 
				case MOVE_UP:  //向上翻页
				case MOVE_LEFT:
					if(pos_cnt>13)pos_cnt-=(13+fcnt);//回到上一页
					else pos_cnt=(total_file%13)?(total_file/13*13):(total_file-13);//从头开始了
					break;
				case MOVE_DOWN://向下翻页
		    	case MOVE_RIGHT:
					if(pos_cnt==total_file)pos_cnt=0;//重新找
					break;

			}	 				    
			fcnt=0;//清除显示文件计数器
		}
	}		
}

//加载MP3播放界面
//sysfun变量。0~9bit用来标记不同状态
void Load_Gui_Mp3(u16 funmark)
{	 
	u16 t;
	u16 temp;
	u16 GraPer;
	u8 n,m,k;
	if(funmark)	goto UDM;	//判断加载标记，若界面已加载过，跳到标题栏绘制
	for(t=0;t<30;t++)  //播放器顶部绘制
	{
		if(t<10) GraPer=t*25;
		else GraPer=t*25+(t-15)*25;	//渐进色百分比计算
		temp=GradualColor(0x0001,0x5AF0,GraPer);
		LCD_Fill(0,t,319,t,temp);   //填充顶部颜色
	}
	POINT_COLOR=0xB577;
	Show_Str_Mid(0,7,"音乐播放器",16,320);
	TFT_CloseButton(290,5,20,0x632E,0x2105); //绘制关闭按钮
	POINT_COLOR=0x20A9;
	LCD_DrawRect(10,30,309,189); //绘制矩形框，达到立体效果
UDM:
	for(t=31;t<44;t++)	//标题栏绘制，显示歌曲名的区域
	{					//在连续音乐播放时，这里需要重绘
		GraPer=(t-31)*85;
		temp=GradualColor(0x52B0,0x420C,GraPer);
		LCD_Fill(11,t,308,t,temp);   //填充顶部颜色
	}
	LCD_Fill(11,44,308,54,0x0863);	 //标题栏下部暗色条
	LCD_Fill(11,57,308,188,0x28CA);	//填充FFT显示区域
	if(funmark) return;	//界面已加载过，则返回

	LCD_Fill(11,55,308,55,0x424D); //标题栏与FFT显示结合处绘制
	temp=GradualColor(0x28CA,0xFFFF,100);
	LCD_Fill(11,56,308,56,temp);	//实现下陷又凸起的立体效果

	LCD_Fill(0,30,9,189,0x5AF0);
	LCD_Fill(310,30,319,189,0x4A6E);//绘制播放器两侧边

	for(t=190;t<300;t++)  //播放器下部渐进颜色
	{
		GraPer=(t-190)*9;
		temp=GradualColor(0x5AF0,0x10C4,GraPer);
		LCD_Fill(0,t,319,t,temp);   
	}
	for(t=206;t<218;t++) //取进度条下方播放时间与比特率字符显示区域
	{					 //背景颜色，在重写该部分字符时使用
		BGC_Buffer[t-206][0]=LCD_ReadPoint(160,t);
	}
	temp=LCD_ReadPoint(30,190);//FFT显示区域下边缘颜色加亮
	temp=GradualColor(temp,0xFFFF,150);
	LCD_Fill(11,190,308,190,temp);

	temp=LCD_ReadPoint(30,195);	//进度条上边缘颜色加深
	temp=GradualColor(temp,0x0000,150);
	LCD_Fill(20,195,299,195,temp);
	temp=LCD_ReadPoint(30,204);//进度条下边缘颜色加亮
	temp=GradualColor(temp,0xFFFF,150);
	LCD_Fill(20,204,299,204,temp);
	for(t=195;t<205;t++)	  //进度条左右两边缘立体效果
	{
		temp=LCD_ReadPoint(19,t);
		POINT_COLOR=GradualColor(temp,0xFFFF,150);
		LCD_DrawPoint(19,t);   //左边缘颜色加亮
		POINT_COLOR=GradualColor(temp,0x0000,150);
		LCD_DrawPoint(300,t);  //右边缘颜色加深
	} 

	for(t=0;t<110;t++)	  //频谱色条渐变计算
	{
		GraPer=t*9;
		temp=GradualColor(0x7DDE,0x5B16,GraPer);	
		FFT_Color[t]=temp;
	}	  
	TFT_DrawCirque(100,255,29,26,0x5A8E);//三个按钮与边缘颜色衔接
	TFT_DrawCirque(35,255,19,16,0x5A8E);
	TFT_DrawCirque(165,255,19,16,0x5A8E);//浅色环绘制

	TFT_DrawCirque(100,255,31,30,0x0002);
	TFT_DrawCirque(35,255,21,20,0x0002);
	TFT_DrawCirque(165,255,21,20,0x0002); //深色环绘制

	TFT_DrawCButton(100,255,25,0xACF9,0x2910,2);//绘制播放按钮
	TFT_DrawCButton(35,255,15,0xACF9,0x2910,3);//绘制上一曲按钮
	TFT_DrawCButton(165,255,15,0xACF9,0x2910,4);//绘制下一曲按钮

	LCD_WriteCom(0x0036);LCD_WriteData(0x002a);	//LCD扫描方式设定,X\Y坐标交换
	LCD_ReadRect(VOLBAR_Y-12,VOLBAR_X,VOLBAR_Y,VOLBAR_X+77,BGV_Color);//读取音量显示区域的背景颜色
																	 //在重绘时要用到
	POINT_COLOR = 0x39EB;
	LCD_SetDisplayWindow(VOLBAR_Y-12,VOLBAR_X,VOLBAR_Y,VOLBAR_X+77);//设定窗口	
	for(n=0;n<26;n++)	 //绘制音量条外框
	{
		for(m=0;m<3;m++)
		for(k=0;k<13;k++)
		{
			if(12-k>n/2)
			LCD->LCD_RAM = BGV_Color[n*39+m*13+k];
			else if(12-k==n/2)
			LCD->LCD_RAM = GradualColor(BGV_Color[n*39+m*13+k],0x0000,200);//加深音量框的上边缘
			else
			LCD->LCD_RAM = POINT_COLOR;
		}
	}

	LCD_ReadRect(VOLBAR_Y-12,VOLBAR_X,VOLBAR_Y,VOLBAR_X+77,BGV_Color);
	LCD_WriteCom(0x0036);LCD_WriteData(0x000a); //读取背景颜色数据
	
	temp=LCD_ReadPoint(VOLBAR_X,VOLBAR_Y+1);
	POINT_COLOR=GradualColor(temp,0xFFFF,150); //加亮音量框的下边缘
	LCD_Fill(VOLBAR_X,VOLBAR_Y+1,VOLBAR_X+77,VOLBAR_Y+1,POINT_COLOR);//实现立体效果
	TFT_DrawSpeak(VOLBAR_X-30,VOLBAR_Y-5);

	TFT_FucButton(200,270,30,0x632E,0x2105,"EQ");//绘制EQ按钮
	TFT_FucButton(240,270,30,0x632E,0x2105,"LRC");//绘制LRC按钮
	TFT_FucButton(280,270,30,0x632E,0x2105,"A");//绘制循环模式按钮
											   //默认全部循环
	LCD_Fill(0,300,319,479,BLACK);	//歌词显示部分
}
/*
//设置音效的界面
//kind:0:VS1003 
//kind:1:PT2314
void Load_Gui_Set_EQ(u8 kind)
{			  
	LCD_Clear(WHITE);//清屏 	   
	//标题栏
	LCD_Fill(0,0,239,20,MP3COLOR1);   //填充顶部颜色
	POINT_COLOR=GRED;
	BACK_COLOR=MP3COLOR1;
	Show_Str_Mid(0,3,"音效管理",16,240);   

	LCD_Fill(0,21,239,41,Bro_MID);  //填充中部部颜色
	LCD_Fill(0,298,239,319,Bro_BTM);//填充底部颜色	    

	LCD_Fill(0,74,239,106,LGRAY); //填充背色
	LCD_Fill(0,138,239,170,LGRAY);//填充背色 
	
	POINT_COLOR=BLUE;
	if(kind) //设置PT2314
	{
		LCD_Fill(0,202,239,234,LGRAY);//填充背色    
		Show_Str(0,50,"音量:",16,0x01);   //叠加模式,非自动换行
		Show_Str(0,82,"高音:",16,0x01);   //叠加模式,非自动换行
		Show_Str(0,114,"低音:",16,0x01);   //叠加模式,非自动换行
	
		Show_Str(0,146,"左声道:",16,0x01);   //叠加模式,非自动换行
		Show_Str(0,178,"右声道:",16,0x01);   //叠加模式,非自动换行
		Show_Str(0,210,"超重音:",16,0x01);   //叠加模式,非自动换行	 
	}else//设置VS1003
	{
		Show_Str(0,50,"高音:",16,0x01);   //叠加模式,非自动换行
		Show_Str(0,82,"高频:",16,0x01);   //叠加模式,非自动换行
		Show_Str(0,114,"低音:",16,0x01);   //叠加模式,非自动换行
	
		Show_Str(0,146,"低频:",16,0x01);   //叠加模式,非自动换行
		Show_Str(0,178,"音量:",16,0x01);   //叠加模式,非自动换行
	}   				    
	//音量 
	POINT_COLOR=BROWN;
	BACK_COLOR=WHITE;
	TFT_ShowButton(100,46,1);//<
	TFT_ShowButton(172,46,0);//>
	//低音
	TFT_ShowButton(100,110,1);//<
	TFT_ShowButton(172,110,0);//>
	//右声道
	TFT_ShowButton(100,174,1);//<
	TFT_ShowButton(172,174,0);//>
	//高音
	BACK_COLOR=LGRAY;
	TFT_ShowButton(100,78,1);//<
	TFT_ShowButton(172,78,0);//>
	//左声道
	TFT_ShowButton(100,142,1);//<
	TFT_ShowButton(172,142,0);//>
	//超重音
	if(kind)
	{
		TFT_ShowButton(100,208,1);//<
		TFT_ShowButton(172,208,0);//>
		POINT_COLOR=RED;
		Show_Str(5,300,"选项",16,0x01);   //叠加模式,非自动换行
	}else
	{
		POINT_COLOR=RED;
		Show_Str(5,300,"确定",16,0x01);   //叠加模式,非自动换行
	}
	//加载选项，返回按钮						   		    

	Show_Str(203,300,"返回",16,0x01); //叠加模式,非自动换行  
} 			
*/
//////////////////////////与LCD显示相关的函数//////////////////////////////////
//画粗线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawDLine(u16 x1, u16 y1, u16 x2, u16 y2)
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
		Draw_Big_Point(uRow,uCol);//画点 
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
//画颜色列表
//该颜色表外框大小为51*37
//分成5*7个颜色块,总共35种颜色
const u16 Color_Tab[3][7]=
{
{0XF800,0X7E00,0X001F,0X0000,0XFFFF,0X8000,0X0400},
{0XFFE0,0X07FF,0XF81F,0X2B12,0X8400,0XFFF0,0XFC10},
{0X0010,0X841F,0X041F,0XFC10,0X87F0,0XF810,0XFC00},
};
//画颜色列表
//该颜色表外框大小为65*29
//分成3*7个颜色块,总共21种颜色
//#define FRAME1 0XEF5B //窗体色 	 
//#define FRAME2 0XDED7 //窗体色	 
void Draw_Color_Table(u16 x,u16 y)
{	
	u8 t,i;	  
	POINT_COLOR=FRAME2;
	for(i=0;i<4;i++) 
	{	 
		LCD_DrawLine(x,y+i*9,x+63,y+i*9);
	}
	for(i=0;i<4;i++) 
	{					    
		LCD_DrawLine(x,y+i*9+1,x+63,y+i*9+1);
	}		    
	for(i=0;i<8;i++) 
	{	 
		LCD_DrawLine(x+9*i,y,x+9*i,y+28);
	}
	for(i=0;i<8;i++)//画小边框
	{					    
		LCD_DrawLine(x+9*i+1,y,x+9*i+1,y+28);
	}
	for(t=0;t<3;t++)
	{
		for(i=0;i<7;i++)
		{
			LCD_Fill(x+2+i*9,y+2+t*9,x+2+i*9+6,y+2+t*9+6,Color_Tab[t][i]);//把多余的去掉
		}
	}								    	  
}










