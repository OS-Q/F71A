#include "sysfile.h"
#include "fat.h"
#include "jpegbmp.h"
//Mini STM32开发板
//系统文件查找代码					  
//正点原子@ALIENTEK
//2010/6/18
				   
u32 PICCLUSTER=0;//图片文件夹地址
u32 MUSCLUSTER=0;//音乐文件夹地址
u32 TXTCLUSTER=0;//电子书文件夹地址	
u32 LOGOCLUSTER=0;//logo图片地址 
u32 sys_ico[13];  //系统图标缓存区!不能篡改!
u32 file_ico[5]; //文件图标缓存区 folder;mus;pic;book;
u32 *Pic_Addr=(u32*)iclip;//注意 0：表示预览图片的地址 1~n，表示1~n块图片的地址
 											    
//系统文件夹		  
const unsigned char *folder[]=
{
"SYSTEM",	//0
"FONT",		//1
"SYSICO",	//2
"GAME",		//3
"LEVEL1",	//4
"LEVEL2",	//5
"LEVEL3",	//6
"MUSIC",	//7
"PICTURE",	//8
"TEXT", 	//9
};		   

//系统文件名定义
const unsigned char *sysfile[]=
{			  
//系统字体图标 0开始
"GB16.SYS",
"GB12.SYS",
"GB24.SYS",
"UNI2GBK.SYS",
//系统文件图标	4开始
"FOLDER.BMP",
"MUS.BMP",
"PIC.BMP",
"BOOK.BMP",
"UNKNOWN.BMP",
//系统主界面图标 9开始
"MUSIC.BMP",
"PICTURE.BMP",
"TEXT.BMP",
"CAMERA.BMP",				   
"GAME.BMP",
"MOUSE.BMP",  			 
"USB.BMP",
"WIRELESS.BMP",
"TIME.BMP",
"BROWSER.BMP",
"COUNTER.BMP", 
"SETTING.BMP",
"CLOCK.BMP", 	  
};

//得到游戏图片编号
void Get_Game_Pic_Str(u8 t,u8 *str)
{
	*str++='I';
	*str++='M';
	*str++='G';
	*str++=(t/10)%10+'0';
	*str++=t%10+'0';
	*str++='.';	  
	*str++='B';		
	*str++='M';
	*str++='P';
	*str='\0';//加入结束符
}

//获取系统文件的存储地址
//次步出错,则无法启动!!!
//返回0，成功。返回其他，相应的系统文件信息未得到	   
u8 SysInfoGet(u8 sel)
{			 		   
	u32 cluster=0;
	u32 syscluster=0;
	u8 t=0;
	u8 size;
	u8 imgx[9];	
	FileInfoStruct t_file;  	  						    	 
	//得到根目录的簇号
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;			   

	if((sel&0xF0)==0x00)//查找系统文件
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//查找system文件夹
		if(t_file.F_StartCluster==0)return 1;//系统文件夹丢失
		syscluster=t_file.F_StartCluster;//保存系统文件夹所在簇号	  	   
		t_file=F_Search(syscluster,(unsigned char *)folder[2],T_FILE);//在system文件夹下查找SYSICO文件夹
		if(t_file.F_StartCluster==0)return 2; 
		cluster=t_file.F_StartCluster;//保存SYSICO文件夹簇号
		for(t=0;t<13;t++)//查找系统图标,九个
		{
			t_file=F_Search(cluster,(unsigned char *)sysfile[t+9],T_BMP);//在SYSICO文件夹下查找系统图标
			sys_ico[t]=t_file.F_StartCluster;
			if(t_file.F_StartCluster==0)return 3;//失败	
		}
		for(t=0;t<5;t++)//查找文件图标,4个
		{
			t_file=F_Search(cluster,(unsigned char *)sysfile[t+4],T_BMP);//在SYSICO文件夹下查找文件图标
			file_ico[t]=t_file.F_StartCluster;
			if(file_ico[t]==0)return 4;//失败 	  
		}
	}
	else if((sel&0xF0)==0x10)//查找音乐文件夹
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[7],T_FILE);//查找MUSIC文件夹
		if(t_file.F_StartCluster==0)return 5;//MUSIC文件夹丢失
		MUSCLUSTER=t_file.F_StartCluster;//MUSIC文件夹所在簇号	 
	}
	else if((sel&0xF0)==0x20)//查找图片文件夹
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[8],T_FILE);//查找PICTURE文件夹
		if(t_file.F_StartCluster==0)return 6;//图片文件夹丢失
		PICCLUSTER=t_file.F_StartCluster;//图片文件夹所在簇号	 
	}
	else if((sel&0xF0)==0x30)
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[9],T_FILE);//查找PICTURE文件夹
		if(t_file.F_StartCluster==0)return 7;//TXT文件夹丢失
		TXTCLUSTER=t_file.F_StartCluster;//TXT文件夹所在簇号	 
	}
	else if((sel&0xF0)==0x80)
	{	 	
		t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//查找system文件夹
		if(t_file.F_StartCluster==0)return 1;//系统文件夹丢失
		syscluster=t_file.F_StartCluster;//保存系统文件夹所在簇号
		size=sel&0x0F;//得到t:3,3*3;4,4*4;5,5*5									  
		t_file=F_Search(syscluster,(unsigned char *)folder[3],T_FILE);//在system文件夹下查找game文件夹
		if(t_file.F_StartCluster==0)return 8;  		
					   
		t_file=F_Search(t_file.F_StartCluster,(unsigned char *)folder[1+size],T_FILE);//在game文件夹下查找LEVEL(sizex-2)文件夹	
		if(t_file.F_StartCluster==0)return 9;//levelx 文件夹丢失
		cluster=t_file.F_StartCluster;
		  	  			   
		size*=size;//取平方
		for(t=0;t<size+1;t++) //X*X图片地址获取
		{
			Get_Game_Pic_Str(t,imgx);//得到图片编号		  
			t_file=F_Search(cluster,imgx,T_BMP);//在LEVEL1文件夹下查找BMP图片
			Pic_Addr[t]=t_file.F_StartCluster;
			if(Pic_Addr[t]==0)return 10;//系统文件夹丢失	 	
		}	 					
	}
	else return 20;
	return 0;//成功
}

u8 LoadLOGO(void)
{
	u32 cluster=0;
	u32 syscluster=0;
	FileInfoStruct t_file;
	//得到根目录的簇号
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;

	t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//查找system文件夹
	if(t_file.F_StartCluster==0)return 1;//系统文件夹丢失
	syscluster=t_file.F_StartCluster;//保存系统文件夹所在簇号
	t_file=F_Search(syscluster,"LOGO.BMP",T_BMP);//在SYSICO文件夹下查找系统图标
	if(t_file.F_StartCluster==0)return 2;//失败	
	LOGOCLUSTER=t_file.F_StartCluster;
	AI_LoadPicFile(&t_file,0,0,319,479);
	return 0;
}

			 






