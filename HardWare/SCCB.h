#ifndef _SCCB_H
#define _SCCB_H

#include <stm32f10x_lib.h>

#define SCCB_SCK_PORT		GPIOB
#define SCCB_SDA_PORT		GPIOB

#define SCCB_SCK_PIN	GPIO_Pin_6
#define SCCB_SDA_PIN	GPIO_Pin_7

#define SIO_C_SET		PBout(6)=1
#define SIO_C_CLR		PBout(6)=0
#define SIO_D_SET		PBout(7)=1
#define SIO_D_CLR		PBout(7)=0

#define SIO_D_IN	{SCCB_SDA_PORT->CRL &= ((u32)0x0FFFFFFF); \
					 SCCB_SDA_PORT->CRL |= ((u32)0x40000000); \
					 PBout(7) = 1;}
#define SIO_D_OUT	{SCCB_SDA_PORT->CRL &= ((u32)0x0FFFFFFF); \
					 SCCB_SDA_PORT->CRL |= ((u32)0x50000000); \
					 PBout(7) = 1;}


#define SIO_D_STATE	    PBin(7)


///////////////////////////////////////////
void InitSCCB(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
u8 SCCBwriteByte(u8 m_data);
u8 SCCBreadByte(void);


#endif /* _SCCB_H */


