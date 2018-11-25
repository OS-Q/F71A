#ifndef __FLASH_H
#define __FLASH_H			    
#include "sys.h"
#include "stm32f10x_lib.h"
//Mini STM32������
//W25X16 �������� 
//����ԭ��@ALIENTEK
//2010/6/13
//V1.0

#define	SPI_FLASH_CS PBout(12)  //ѡ��FLASH	
				 
////////////////////////////////////////////////////////////////////////////
//W25X16��д
#define FLASH_ID 0X1C14
//ָ���
#define EN25F16_WriteEnable		    0x06 
#define EN25F16_WriteDisable		0x04 
#define EN25F16_ReadStatusReg		0x05 
#define EN25F16_WriteStatusReg	    0x01 
#define EN25F16_ReadData			0x03 
#define EN25F16_FastReadData		0x0B 
#define EN25F16_PageProgram		    0x02 
#define EN25F16_BlockErase		    0xD8 
#define EN25F16_SectorErase		    0x20 
#define EN25F16_ChipErase		    0xC7 
#define EN25F16_PowerDown		    0xB9 
#define EN25F16_ReleasePowerDown    0xAB 
#define EN25F16_DeviceID			0xAB 
#define EN25F16_ManufactDeviceID	0x90 
#define EN25F16_ReadIdentification  0x9F
#define EN25F16_NOP                 0xFF  

u16 SPI_Flash_Init(void);			 //����flash״̬
u8 SPI2_ReadWriteByte(u8 TxData);	 //��дһ���ֽ�
u16  SPI_Flash_ReadID(void);  	    //��ȡFLASH ID
u8	 SPI_Flash_ReadSR(void);        //��ȡ״̬�Ĵ��� 
void SPI_FLASH_Write_SR(u8 sr);  	//д״̬�Ĵ���
void SPI_FLASH_Write_Enable(void);  //дʹ�� 
void SPI_FLASH_Write_Disable(void);	//д����
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void SPI_Flash_Erase_Chip(void);    	  //��Ƭ����
void SPI_Flash_Erase_Block(u32 Dst_Addr); //�����
void SPI_Flash_Erase_Sector(u32 Dst_Addr);//��������
void SPI_Flash_Wait_Busy(void);           //�ȴ�����
void SPI_Flash_PowerDown(void);           //�������ģʽ
u8 SPI_Flash_WAKEUP(void);			  //����
#endif



