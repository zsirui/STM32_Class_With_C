/*
* @Author: Zheng Sirui
* @Filename: iic.h
* @Email: zsirui@126.com
* @Date:   2018-04-08 15:52:23
* @Last Modified by:   Zheng Sirui
* @Last Modified time: 2018-06-12 16:23:25
*/
#ifndef _IIC_H_
#define _IIC_H_
#include "stm32f4xx_conf.h"

typedef struct _IIC_IO_
{
	uint8_t SDA_GPIO_PinSource;
	uint32_t RCC_APBPeriph_No;
	GPIO_InitTypeDef GPIO_SCL;
	GPIO_InitTypeDef GPIO_SDA;
	GPIO_TypeDef* GPIO_SCL_Port;
	GPIO_TypeDef* GPIO_SDA_Port;
	void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
}IIC_IO;

typedef struct _IIC_
{
	IIC_IO IIC_1;
	void (*init)(IIC_IO _IIC);
	void (*deinit)(IIC_IO _IIC);
	void (*IIC_Start)(IIC_IO _IIC);
	void (*IIC_Stop)(IIC_IO _IIC);
	void (*IIC_Send_Byte)(uint8_t txd, IIC_IO _IIC);
	uint8_t (*IIC_Read_Byte)(unsigned char ack, IIC_IO _IIC);
	uint8_t (*IIC_Wait_Ack)(IIC_IO _IIC);
	void (*IIC_Ack)(IIC_IO _IIC);
	void (*IIC_NAck)(IIC_IO _IIC);
}IIC_Class;

extern IIC_Class IIC;

#endif
