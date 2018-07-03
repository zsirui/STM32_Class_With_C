#ifndef _MLX_H_
#define _MLX_H_

#include "stm32f4xx_conf.h"
#include "delay.h"

#define		PORT_1				GPIOE

#define		SDA1				GPIO_Pin_8
#define		SCL1				GPIO_Pin_9
#define		SCL2				GPIO_Pin_7

#define		PORT_2				GPIOF

#define		SDA3				GPIO_Pin_15
#define		SDA4				GPIO_Pin_13
#define		SCL4				GPIO_Pin_14

#define		PORT_3				GPIOG

#define		SDA2				GPIO_Pin_1
#define		SCL3				GPIO_Pin_0

#define 	ACK	 				0						//应答
#define		NACK 				1						//不应答

#define 	RAM 				0x20 					//对RAM进行操作
#define 	EEPROM 				0x10					//对EEPROM进行操作
#define 	SLEEP 				0xC6					//进入睡眠模式
#define 	RD 					0x01 					//读操作
#define 	WR 					0x00 					//写操作
//-- MLX90615 RAM 地址 --
#define 	OBJTEMPADDR			0x07
#define 	ENVITEMPADDR		0x06
//-- MLX90615 EEPROM 地址 --
#define 	PWMTMINADDR			0x00
#define 	SLAVEADDR			0x00
#define 	PWMTRANGEADDR		0x01
#define 	CONFIGADDR			0x02
#define 	EMISSIVITYADDR		0x03
//-- MLX90615 默认值 --
#define 	MLX_ADDR			0x5B
#define 	DEFAULTPWMTMIN		0x355B
#define 	DEFAULTPWMTRANGE	0x09C4
#define 	DEFAULTEMISSIVITY	0x4000

typedef struct
{
	uint8_t SDA_GPIO_PinSource;
	uint32_t RCC_APBPeriph_No;
	GPIO_InitTypeDef GPIO_SCL;
	GPIO_InitTypeDef GPIO_SDA;
	GPIO_TypeDef* GPIO_SCL_Port;
	GPIO_TypeDef* GPIO_SDA_Port;
	void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
}MLX_IO;

typedef struct
{
	MLX_IO MLX_1;
	MLX_IO MLX_2;
	MLX_IO MLX_3;
	MLX_IO MLX_4;
	void (*init)(MLX_IO _MLX);
	void (*deinit)(MLX_IO _MLX);
	uint8_t (*SMBus_ReadTemp_TOBJ)(float *temp, MLX_IO _MLX);
	uint8_t (*SMBus_ReadTemp_surr)(float *temp, MLX_IO _MLX);
	uint8_t (*SMBus_WriteSlaveAddress)(uint16_t addr, MLX_IO _MLX);
}MLX_Class;

extern MLX_Class MLX;

#endif
