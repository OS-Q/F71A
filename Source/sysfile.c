#include "sysfile.h"
#include "fat.h"
#include "jpegbmp.h"
//Mini STM32������
//ϵͳ�ļ����Ҵ���					  
//����ԭ��@ALIENTEK
//2010/6/18
				   
u32 PICCLUSTER=0;//ͼƬ�ļ��е�ַ
u32 MUSCLUSTER=0;//�����ļ��е�ַ
u32 TXTCLUSTER=0;//�������ļ��е�ַ	
u32 LOGOCLUSTER=0;//logoͼƬ��ַ 
u32 sys_ico[13];  //ϵͳͼ�껺����!���ܴ۸�!
u32 file_ico[5]; //�ļ�ͼ�껺���� folder;mus;pic;book;
u32 *Pic_Addr=(u32*)iclip;//ע�� 0����ʾԤ��ͼƬ�ĵ�ַ 1~n����ʾ1~n��ͼƬ�ĵ�ַ
 											    
//ϵͳ�ļ���		  
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

//ϵͳ�ļ�������
const unsigned char *sysfile[]=
{			  
//ϵͳ����ͼ�� 0��ʼ
"GB16.SYS",
"GB12.SYS",
"GB24.SYS",
"UNI2GBK.SYS",
//ϵͳ�ļ�ͼ��	4��ʼ
"FOLDER.BMP",
"MUS.BMP",
"PIC.BMP",
"BOOK.BMP",
"UNKNOWN.BMP",
//ϵͳ������ͼ�� 9��ʼ
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

//�õ���ϷͼƬ���
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
	*str='\0';//���������
}

//��ȡϵͳ�ļ��Ĵ洢��ַ
//�β�����,���޷�����!!!
//����0���ɹ���������������Ӧ��ϵͳ�ļ���Ϣδ�õ�	   
u8 SysInfoGet(u8 sel)
{			 		   
	u32 cluster=0;
	u32 syscluster=0;
	u8 t=0;
	u8 size;
	u8 imgx[9];	
	FileInfoStruct t_file;  	  						    	 
	//�õ���Ŀ¼�Ĵغ�
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;			   

	if((sel&0xF0)==0x00)//����ϵͳ�ļ�
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//����system�ļ���
		if(t_file.F_StartCluster==0)return 1;//ϵͳ�ļ��ж�ʧ
		syscluster=t_file.F_StartCluster;//����ϵͳ�ļ������ڴغ�	  	   
		t_file=F_Search(syscluster,(unsigned char *)folder[2],T_FILE);//��system�ļ����²���SYSICO�ļ���
		if(t_file.F_StartCluster==0)return 2; 
		cluster=t_file.F_StartCluster;//����SYSICO�ļ��дغ�
		for(t=0;t<13;t++)//����ϵͳͼ��,�Ÿ�
		{
			t_file=F_Search(cluster,(unsigned char *)sysfile[t+9],T_BMP);//��SYSICO�ļ����²���ϵͳͼ��
			sys_ico[t]=t_file.F_StartCluster;
			if(t_file.F_StartCluster==0)return 3;//ʧ��	
		}
		for(t=0;t<5;t++)//�����ļ�ͼ��,4��
		{
			t_file=F_Search(cluster,(unsigned char *)sysfile[t+4],T_BMP);//��SYSICO�ļ����²����ļ�ͼ��
			file_ico[t]=t_file.F_StartCluster;
			if(file_ico[t]==0)return 4;//ʧ�� 	  
		}
	}
	else if((sel&0xF0)==0x10)//���������ļ���
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[7],T_FILE);//����MUSIC�ļ���
		if(t_file.F_StartCluster==0)return 5;//MUSIC�ļ��ж�ʧ
		MUSCLUSTER=t_file.F_StartCluster;//MUSIC�ļ������ڴغ�	 
	}
	else if((sel&0xF0)==0x20)//����ͼƬ�ļ���
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[8],T_FILE);//����PICTURE�ļ���
		if(t_file.F_StartCluster==0)return 6;//ͼƬ�ļ��ж�ʧ
		PICCLUSTER=t_file.F_StartCluster;//ͼƬ�ļ������ڴغ�	 
	}
	else if((sel&0xF0)==0x30)
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[9],T_FILE);//����PICTURE�ļ���
		if(t_file.F_StartCluster==0)return 7;//TXT�ļ��ж�ʧ
		TXTCLUSTER=t_file.F_StartCluster;//TXT�ļ������ڴغ�	 
	}
	else if((sel&0xF0)==0x80)
	{	 	
		t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//����system�ļ���
		if(t_file.F_StartCluster==0)return 1;//ϵͳ�ļ��ж�ʧ
		syscluster=t_file.F_StartCluster;//����ϵͳ�ļ������ڴغ�
		size=sel&0x0F;//�õ�t:3,3*3;4,4*4;5,5*5									  
		t_file=F_Search(syscluster,(unsigned char *)folder[3],T_FILE);//��system�ļ����²���game�ļ���
		if(t_file.F_StartCluster==0)return 8;  		
					   
		t_file=F_Search(t_file.F_StartCluster,(unsigned char *)folder[1+size],T_FILE);//��game�ļ����²���LEVEL(sizex-2)�ļ���	
		if(t_file.F_StartCluster==0)return 9;//levelx �ļ��ж�ʧ
		cluster=t_file.F_StartCluster;
		  	  			   
		size*=size;//ȡƽ��
		for(t=0;t<size+1;t++) //X*XͼƬ��ַ��ȡ
		{
			Get_Game_Pic_Str(t,imgx);//�õ�ͼƬ���		  
			t_file=F_Search(cluster,imgx,T_BMP);//��LEVEL1�ļ����²���BMPͼƬ
			Pic_Addr[t]=t_file.F_StartCluster;
			if(Pic_Addr[t]==0)return 10;//ϵͳ�ļ��ж�ʧ	 	
		}	 					
	}
	else return 20;
	return 0;//�ɹ�
}

u8 LoadLOGO(void)
{
	u32 cluster=0;
	u32 syscluster=0;
	FileInfoStruct t_file;
	//�õ���Ŀ¼�Ĵغ�
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;

	t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//����system�ļ���
	if(t_file.F_StartCluster==0)return 1;//ϵͳ�ļ��ж�ʧ
	syscluster=t_file.F_StartCluster;//����ϵͳ�ļ������ڴغ�
	t_file=F_Search(syscluster,"LOGO.BMP",T_BMP);//��SYSICO�ļ����²���ϵͳͼ��
	if(t_file.F_StartCluster==0)return 2;//ʧ��	
	LOGOCLUSTER=t_file.F_StartCluster;
	AI_LoadPicFile(&t_file,0,0,319,479);
	return 0;
}

			 






