#include "fat.h"
#include "gui.h"
#include "lcd.h"
#include "flash.h"
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "vs1003.h"
#include "jpegbmp.h"
#include "sdcard.h"
#include "mp3player.h"
  

u16 f_kbps=0;//歌曲文件位率	

//进度条,时间,比特率 等事实信息显示控制
void MP3_PROG(u32 pos,u32 lenth)
{	
	static u16 playtime;//播放时间标记
	static u16 alltime;	     
	u32 propos;	
	u16 time;	// 时间变量
	POINT_COLOR=0x37E6;	

	if(lenth)
	{		
		propos=(pos*1000)/(lenth>>9); //得到百分比 	
		if(propos>1000)propos=1000;//限制propos的范围
	}else return;//文件为空,返回
	 
	TFT_ProBar((u16)(propos*280/1000));//显示进度条  
	
	if(f_kbps==0xffff)//未更新过
	{
		playtime=0;
		f_kbps=GetHeadInfo();	   //获得比特率
		if(f_kbps) alltime=(lenth/f_kbps)/125; //总时间
		else alltime=0;   //非法比特率

		LCD_DisplayChar(189,206,':',12,0x10); //播放时间的间隔符‘:’
		LCD_DisplayChar(207,206,'/',12,0x10);//播放时间与总时间的间隔‘/’
		//显示总时间
		LCD_Show2Num(213,206,alltime/60,12,0x10);//显示总分钟数
		LCD_DisplayChar(225,206,':',12,0x10); //总时间的间隔符‘:’ 	  
		LCD_Show2Num(231,206,alltime%60,12,0x10);//显示总秒钟数  
		//显示位率			   
		LCD_Show3Num(258,206,f_kbps,12,0x10);
		LCD_DisplayChar(276,206,'k',12,0x10);
		LCD_DisplayChar(282,206,'b',12,0x10);
		LCD_DisplayChar(288,206,'p',12,0x10);
		LCD_DisplayChar(294,206,'s',12,0x10);
	}
	      
  	if(f_kbps)//对位率不为0 的文件
	time=propos*alltime/1000;  //得到当前比例时间
	else time=GetDecodeTime(); //对位率为0的文件,得到解码时间
 			  
	if(time!=playtime)//1s时间到,更新显示数据
	{
		playtime=time;//更新时间 	 		 
		//显示播放时间			 
		LCD_Show2Num(177,206,time/60,12,0x10); //分钟	   		   
		LCD_Show2Num(195,206,time%60,12,0x10);//秒钟 	 				
	}   		 
}
/*
u8 *MP3_VOL[5]={" 音量 "," 高音 "," 低音 ","左声道","右声道"};
u8 *MP3_OTHER[3]={"频谱显示","歌词显示","退出播放"};
 		  
//检测数据合法性
void VOL_Data_Check(u8 volx,u8 *datax)
{
	switch(volx)
	{
		case 0:	//主音量
			if(*datax>64)*datax=64;
			if(*datax<1)*datax=1;
			break;
		case 1:	//高音			  
		case 2:	//低音
			if(*datax>15)*datax=15;
			if(*datax<1)*datax=1;
			break;
		case 3: //左声道
		case 4:	//右声道
			if(*datax>32)*datax=32;
			if(*datax<1)*datax=1;
			break;
		case 5://超重音
			if(*datax>1)*datax=1;
			break;		    
	}
}*/
/*
//显示数据
void MP3_MsgBox_Data(u16 *sysmsg,u8 menu,u8 key)
{
	u16 volx;	   
	BACK_COLOR=MP3COLOR6;//背景色
	POINT_COLOR=RED;
	LCD_Fill(58,259,138,275,MP3COLOR6);//抹掉上次数据
	switch(menu)
	{
		case 0://音效模式		 
			if(key==7) INITDSP++;	 
			else if(key==8)
			{
				if(INITDSP==0)INITDSP=4;//数据合法性判断
				else INITDSP--; 
			}					    	
			if(INITDSP>4)INITDSP=0;				   
			LCD_Show2Num(82,259,INITDSP+1,16,0);//显示数据
			break;
		case 1:
			volx=(*sysmsg>>2)&0x07;
			if(key==7)voltemp[volx]++;	 
			else if(key==8)voltemp[volx]--; 
			VOL_Data_Check(volx,&voltemp[volx]);//数据合法性判断  
			LCD_Show2Num(82,259,voltemp[volx],16,0);//显示数据
			break;
		case 2:	
			volx=(*sysmsg>>5)&0x03;	 
			if(key==7){volx++;if(volx>2)volx=0;}	 
			else if(key==8) 
			{
				if(volx)volx--;
				else volx=2;
			} 				    						    	 
			*sysmsg&=0XFF9F;
			*sysmsg|=volx<<5; 					   
			Show_Str(58,259,MP3_OTHER[volx],16,0);
			break;   
	}
	//第三栏信息更新
	BACK_COLOR=MP3COLOR3;//背景色
	POINT_COLOR=GRED;
	for(volx=0;volx<3;volx++)LCD_Show2Num(26+60*volx,47,voltemp[volx],16,0);//显示音量信息  
	LCD_Show2Num(206,47,INITDSP+1,16,0);//显示音效信息  
}
*/

//显示MP3目录表的信息
//*menu	  :先前的目录号
//*sysmsg :但前运行标志信息
//curmenu:当前目录号   
/*
void MP3_MsgBox_Menu(u16 *sysmsg,u8 *menu,u8 curmenu)
{
	u16 temp;   	 
	BACK_COLOR=MP3COLOR6;  
	//显示音效部分
	temp=(*sysmsg>>9)&0x01;
	if(curmenu==0)
	{
		POINT_COLOR=RED;  
		if(*menu==curmenu)//再次点击
		{
			temp=!temp;								  
			*sysmsg&=0XFDFF;
			*sysmsg|=temp<<9;
		}
		*menu=curmenu;
	}	 						
	else POINT_COLOR=GREEN;	  
	if(temp)Show_Str_Mid(8,227,"存音效",16,48);
	else Show_Str_Mid(8,227," 音效 ",16,48); 
	//显示音量
	temp=(*sysmsg>>2)&0x07;	   
	if(curmenu==1)
	{
		POINT_COLOR=RED;
		if(*menu==curmenu)//再次点击
		{
			temp++;
			if(temp>=5)temp=0;						   	 
			*sysmsg&=0XFFE3;
			*sysmsg|=temp<<2;
			temp=(*sysmsg>>2)&0x07;
		}
		*menu=curmenu;
	}	 
	else POINT_COLOR=GREEN;	  
	Show_Str_Mid(64,227,MP3_VOL[temp],16,48);
	//其他选择			    	 
	if(curmenu==2)
	{
		*menu=curmenu;
		POINT_COLOR=RED;  	 
	}
	else POINT_COLOR=GREEN;	 
	Show_Str_Mid(120,227," 其他 ",16,48);	 
}*/
							  	 
//播放选定的歌曲 					  
//index:当前播放的歌曲索引
//total:总共音乐文件的个数
//关于音乐的几乎所有控制都在这个里面
u8 Mus_Play(FileInfoStruct *FileName,u16 index,u16 total)
{	 
    unsigned long bfactor; //物理扇区,绝对地址
    unsigned long bcluster;//物理簇号
	u32 file_pos;          //记录文件播放的长度
	u8 forpreclust;//快进快退需要的簇数 
    u8 count; 		//扇区计数器,用于换簇

 	u8 *BUFFER=(u8*)MCUBuffer;//指向JPEG的开始512字节
	u8 *spedate=(u8*)MCUBuffer+512;//当前频谱分析的结果寄存器
  	u8 *topval=(u8*)MCUBuffer+526; //FFT柱的最高点记录表
	u8 *flyval=(u8*)MCUBuffer+540; //FFT柱的悬浮点记录表
	u32 topstay1;   //最高点频点延时1
	u32 topstay2;	//最高点频点延时2
			  	   
	u8 load=0;	 //界面加载标记，连续播放歌曲时，不需要每次重加载界面
	u8 mark=1;	//更新FFT显示/读取FFT数据标记
				//因为读取FFT后VS1003已进入就绪状态，需要立刻续传数据
				//更新显示需下一轮循环进行	
	u8 timer;	//频谱定时器
	u16 temp;	//暂存寄存器
	u16 i,n;	//循环计数用
	u8 t,key,keycnt;	//按键计数器
	u8 FFT_LRC=0;//默认FFT
	//bit0~9用来标记功能 ,默认全部循环
	//bit0,1:循环模式;bit2,3,4,5,6:音量设置;bit7:EQ特效标记;
	//bit8:是否有歌词;bit9:音效存/取标记;bit10:播放/暂停标记
	u16 sysfun=1;		    									 
 	 		   															 
/*	timer3_init();//初始化歌词显示定时器*/
	Read_VS_Set();//读取VS1003的设置
	Mp3Reset();       //硬复位
	Vs1003SoftReset();//软复位VS1003
	LoadPatch();      //加载频谱分析代码 
 	Load_Gui_Mp3(load);//加载播放界面
	TFT_Volume(GetVolume());

RESTART:
	if(load)
	{
		Vs1003SoftReset();//软复位VS1003
		LoadPatch();     //加载频谱分析代码
		Load_Gui_Mp3(load);
	}
	else load=1;	
	//fft部分,变量清空
	topstay1=0;
	topstay2=0;
	file_pos=0;	  
	for(i=0;i<14;i++)//清空相关数据
	{
		topval[i]=0;		 	    
		flyval[i]=0;
		spedate[i]=0;
	}		//FFT显示变量数据						  
	key=0;t=0;
	keycnt=0;
	f_kbps=0xffff;//文件位率设置为不可能值
	timer=5;	//设置到一个不可能的值,初始化信息栏	
	count=0;	//扇区计数器

	FAT_TAB.Fat_Head_Pos=0;
	Copy_Fat_Table(FileName->F_StartCluster); 

    bfactor=fatClustToSect(FileName->F_StartCluster);//得到开始簇对应的扇区
	bcluster=FileName->F_StartCluster;		//得到文件开始簇号	
	
	forpreclust=FileName->F_Size/(SectorsPerClust*BytesPerSector*50);//每次前进/后退 2%所需要的簇数	   
	if(forpreclust<2)forpreclust=2;//最少也要两个簇
	  
	//从当前目录下找歌词文件,有,则标记.	 
/*	if(FindLrcFile(Cur_Dir_Cluster,FileName->F_ShortName))
	{								   
		sysfun|=1<<8;     //标记有歌词
		Lrc_Read_Init();  //初始化歌词读取
	}else */
	sysfun&=~(1<<8);//标记没有歌词	 							 					 
	//显示但前文件夹下的歌曲数目,及当前歌曲的索引
	POINT_COLOR=GradualColor(0xFFFF,0x0000,200); 
	LCD_Show3Num(246,34,index,16,1);
	LCD_DisplayChar(270,34,'/',16,1); 
	LCD_Show3Num(278,34,total,16,1); 
	//显示歌曲标题
	Show_Str(15,34,FileName->F_Name,16,1); //显示歌曲名字
	//FFT数据清空			 	    
	Pen_Point.Key_Sta=Key_Up;//释放按键
				
	while(1)   //播放音乐的主循环
    {
        if(SD_ReadBlock(bfactor<<9,(u32 *)BUFFER,512) != SD_OK)break;//读取一个扇区的数据 
        i=0;  	  
        count++;//扇区计数器 
		do      //主播放循环
        {  
			while(VS_DREQ==0&&((sysfun&0X0400)==0)) //等待高/进行频谱显示/非暂停 
			{
				if(FFT_LRC==0)//频谱模式
				{
					if(mark)
					{
						GetSpec(spedate);//得到频谱数据
						MP3_PROG(file_pos,FileName->F_Size);//MP3进度显示/播放时间  
					}
					else TFT_ShowFFT(spedate,topval,flyval,&topstay1,&topstay2);//频谱显示     	  
					timer++; 
					if(timer>=3)//FFT最高的悬浮点控制 
					{
						Top_Delet_FFT(flyval,&topstay1,&topstay2);//删除高端
						timer=0; 
					}   
				}else 	    //歌词模式
				{					   											    
				/*	if(mark&&sysfun&(1<<8))LyricDisplayCtrl();//存在歌词,则显示
					else if(mark==0)MP3_PROG(file_pos,FileName->F_Size);//MP3进度显示/播放时间 				  
					//printf("end\n"); */
				}	
				mark=!mark;
			}	 
			if(VS_DREQ!=0&&((sysfun&0X0400)==0))  //非暂停,送数据给VS1003
			{			   	 
	            for(n=0;n<32;n++)Vs1003_DATA_Write(BUFFER[i++]);//发送音乐数据    
			}
			key=AI_Read_TP(1,1);	    
			if(key) t=Touch_To_Num(3);
			else Pen_Point.Key_Reg=0;	   		 
			
			if(t)//有按键按下
			{
				switch(t)
				{
					case 1://循环模式更改
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							temp=sysfun&0x03;
							temp++;
							if(temp>1)temp=0;//只用其中两个,单曲循环/全部循环
							//显示循环模式
							if(temp)
							TFT_FucButton(280,270,30,0x632E,0x2105,"A");
							else
							TFT_FucButton(280,270,30,0x632E,0x2105,"S");		   
							sysfun&=0XFFFC;
							sysfun|=temp;//更改标志位
						}
						Pen_Point.Key_Reg|=0X80; //标记按键已经按下	
						break;
					case 2://EQ 部分
						if((Pen_Point.Key_Reg&0x80)==0)
						{
						}
						Pen_Point.Key_Reg|=0X80; //标记按键已经按下
						break;
					case 3://LRC部分
						if((Pen_Point.Key_Reg&0x80)==0)
						{
						}
						Pen_Point.Key_Reg|=0X80; //标记按键已经按下
						break;    
					case 4://音量部分 
						sysfun |= 0x0200;					 	  
						TFT_Volume((Pen_Point.X-VOLBAR_X)/3);
						SetVolume((Pen_Point.X-VOLBAR_X)/3);
						break;
					case 5://关闭
						if(sysfun&0x0200)
						Save_VS_Set();
						Mp3Reset();
						return 0xFF;
					case 6://上一曲
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							index--;
							if(index==0)index=total;
							Get_File_Info(Cur_Dir_Cluster,FileName,T_MP3|T_WMA|T_WAV|T_MID,&index);
							goto RESTART;
						}
						Pen_Point.Key_Reg|=0X80; //标记按键已经按下
						break;					
					case 7://播放暂停
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							temp=(sysfun>>10)&0x01;
							temp=!temp;	  	   
							if(temp) TFT_DrawCButton(100,255,25,0xACF9,0x2910,1);//播放
							else TFT_DrawCButton(100,255,25,0xACF9,0x2910,2);//暂停						    
							sysfun&=0XFBFF;
							sysfun|=temp<<10;
						}
						Pen_Point.Key_Reg|=0X80; //标记按键已经按下
						break;
					case 8://下一曲
						if((Pen_Point.Key_Reg&0x80)==0)
						{
							index++;
							if(index>total)index=1;
							Get_File_Info(Cur_Dir_Cluster,FileName,T_MP3|T_WMA|T_WAV|T_MID,&index);
							goto RESTART;
						}
						Pen_Point.Key_Reg|=0X80; //标记按键已经按下	
						break;	 
					case 9://快进或者快退 
                        if(keycnt>2)   
                        {	   
							temp=((float)file_pos*512/FileName->F_Size)*280;//得到百分比 
							if(temp+22<Pen_Point.X) //当前进度小于指定位置,快进
							{
	                            for(i=0;i<forpreclust;i++)//向后偏移5个簇
	                            {
	                                bcluster=FatTab_Next_Cluster(bcluster);//偏移一个簇
	                                if((FAT32_Enable==0&&bcluster==0xffff)||bcluster==0x0ffffff8||bcluster==0x0fffffff) 
	                                {
	                                    bcluster=FatTab_Prev_Cluster(bcluster);//上一个簇	   
										break;
	                                }
	                            } 	
								file_pos+=SectorsPerClust*i; //文件游标递增
							}else if(temp+18>Pen_Point.X) //一个不太好的快退算法.
							{
	                            for(i=0;i<forpreclust;i++)//向前偏移5个簇
	                            {
	                                bcluster=FatTab_Prev_Cluster(bcluster);//向前一簇
	                                if((FAT32_Enable==0&&bcluster==0xffff)||bcluster==0x0ffffff8||bcluster==0x0fffffff) 
	                                {
	                                    bcluster=FatTab_Next_Cluster(FileName->F_StartCluster);//向后一个簇	   
										break;
	                                }
	                            }
								if(bcluster==FileName->F_StartCluster) file_pos=0;								
								else file_pos-=SectorsPerClust*i;//文件游标递减						
							}else break;//在设定范围内,不需要处理 

							bfactor=fatClustToSect(bcluster);//得到当前应播放的扇区	    
							bfactor--;//为后面扇区减抵消一下	  
                            
							count=0; 
							i=512;    //跳出while用
							MP3_PROG(file_pos,FileName->F_Size);//MP3进度显示/播放时间  	  
                        }else keycnt++;	//累积按键次数  	  			
						break;  
				}  
				t=0;			  						  
			}  		           
        }while(i<511);//循环发送512个字节 
		file_pos++;//增加512个字节	  
        bfactor++; //扇区加
        if(count>=SectorsPerClust)//一个簇结束,换簇
        {
            count=0;  		   
            bcluster=FatTab_Next_Cluster(bcluster);
			//文件结束 
			if((FAT32_Enable==0&&bcluster==0xffff)||bcluster==0x0ffffff8||bcluster==0x0fffffff)break;//error	
            bfactor=fatClustToSect(bcluster);
	    }
		if(file_pos%(forpreclust*SectorsPerClust)==1)
		SignalLED=0;
		else SignalLED=1; 
    } 				   
	temp=sysfun&0x03;//0/1				 
	if(temp==1)//全部循环
	{
		index++; 						   
    	if(index>total)index=1;
		Get_File_Info(Cur_Dir_Cluster,FileName,T_MP3|T_WMA|T_WAV|T_MID,&index);			  
	}							   
	goto RESTART;//重新回到那里播放,单曲/全部循环	 
}

//音乐浏览器
//支持FAT32/16文件系统的浏览.
//check:09/04/18
void Mus_Viewer(void)
{		      
	FileInfoStruct *FileInfo;		 
	u16 mus_cnt=0;//当前目录下音乐文件的个数
	u16 index=0;  //当前选择的文件编号	   
	//从根目录开始浏览
	if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//根目录簇号
 	else Cur_Dir_Cluster=0;						   
	Fat_Dir_Cluster=Cur_Dir_Cluster;//父目录就是当前目录
	while(1)
	{
BROS:	 	     
		Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发
		Load_Gui_Bro("选择");//显示GUI界面
		POINT_COLOR=WHITE;	
		Show_Str(128,2,"音乐浏览",16,0x01);//叠加模式,非自动换行
			
		index=File_Browse(T_MP3|T_WMA|T_WAV|T_MID);//浏览文件夹    
		if(!index)return;//结束浏览,退出此功能!!!!!

		mus_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_MP3|T_WMA|T_WAV|T_MID,&mus_cnt);//获取当前文件夹下面的目标文件个数 
		Pen_Point.Key_Sta=Key_Up;// 放显示期间的触发 
		FileInfo=&F_Info[0];//开辟暂存空间.
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_MP3|T_WMA|T_WAV|T_MID,&index);//得到这首歌曲的信息	 	  
			Mus_Play(FileInfo,index,mus_cnt);			   			 	   
			goto BROS;	   
		}
	}			 
}



















