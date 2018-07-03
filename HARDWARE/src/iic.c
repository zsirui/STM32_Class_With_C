/*
* @Author: Zheng Sirui
* @Filename: iic.c
* @Email: zsirui@126.com
* @Date:   2018-04-08 15:52:23
* @Last Modified by:   Zheng Sirui
* @Last Modified time: 2018-06-12 16:23:25
*/
#include "iic.h"
#include "delay.h"

static void SDA_INPUT(GPIO_TypeDef* GPIOx, uint8_t GPIO_PinSource)
{
	GPIOx->MODER &= ~((unsigned int)3 << (GPIO_PinSource * 2));
	GPIOx->MODER |= (0 << (GPIO_PinSource * 2));
}

static void SDA_OUTPUT(GPIO_TypeDef* GPIOx, uint8_t GPIO_PinSource)
{
	GPIOx->MODER &= ~((unsigned int)3 << (GPIO_PinSource * 2));
	GPIOx->MODER |= (1 << (GPIO_PinSource * 2));
}

static void _IIC_Start(IIC_IO _IIC)
{
	SDA_OUTPUT(_IIC.GPIO_SDA_Port, _IIC.SDA_GPIO_PinSource);
	GPIO_SetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
	GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	delay_us(4);
	GPIO_ResetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
	delay_us(4);
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
}

static void _IIC_Stop(IIC_IO _IIC)
{
	SDA_OUTPUT(_IIC.GPIO_SDA_Port, _IIC.SDA_GPIO_PinSource);
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	GPIO_ResetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
	delay_us(4);
	GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	GPIO_SetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
	delay_us(4);
}

static void init(IIC_IO _IIC)
{
	_IIC.RCC_APBPeriphClockCmd(_IIC.RCC_APBPeriph_No, ENABLE);
	GPIO_Init(_IIC.GPIO_SCL_Port, &_IIC.GPIO_SCL);
	GPIO_Init(_IIC.GPIO_SDA_Port, &_IIC.GPIO_SDA);
	GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	GPIO_SetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
}

static uint8_t IIC_Wait_Ack(IIC_IO _IIC)
{
	u8 ucErrTime = 0;
	SDA_INPUT(_IIC.GPIO_SDA_Port, _IIC.SDA_GPIO_PinSource);
	GPIO_SetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
	delay_us(1);
	GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	delay_us(1);
	while(GPIO_ReadInputDataBit(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin))
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			_IIC_Stop(_IIC);
			return 1;
		}
	}
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	return 0;
}

static void IIC_Ack(IIC_IO _IIC)
{
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	SDA_OUTPUT(_IIC.GPIO_SDA_Port, _IIC.SDA_GPIO_PinSource);
	GPIO_ResetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
	delay_us(2);
	GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	delay_us(2);
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
}

static void IIC_NAck(IIC_IO _IIC)
{
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	SDA_OUTPUT(_IIC.GPIO_SDA_Port, _IIC.SDA_GPIO_PinSource);
	GPIO_SetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
	delay_us(2);
	GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	delay_us(2);
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
}
static void IIC_Send_Byte(uint8_t txd, IIC_IO _IIC)
{
	uint8_t t;
	SDA_OUTPUT(_IIC.GPIO_SDA_Port, _IIC.SDA_GPIO_PinSource);
	GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
	for(t = 0; t < 8; t++)
	{
		if((txd & 0x80) >> 7 == 1)
		{
			GPIO_SetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
		} else 
		{
			GPIO_ResetBits(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin);
		}
		txd <<= 1;
		delay_us(2);
		GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
		delay_us(2);
		GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
		delay_us(2);
	}
}

static uint8_t IIC_Read_Byte(unsigned char ack, IIC_IO _IIC)
{
	unsigned char i, receive = 0;
	SDA_INPUT(_IIC.GPIO_SDA_Port, _IIC.SDA_GPIO_PinSource);
	for(i = 0; i < 8; i++ )
	{
		GPIO_ResetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
		delay_us(2);
		GPIO_SetBits(_IIC.GPIO_SCL_Port, _IIC.GPIO_SCL.GPIO_Pin);
		receive <<= 1;
		if(GPIO_ReadInputDataBit(_IIC.GPIO_SDA_Port, _IIC.GPIO_SDA.GPIO_Pin))
		{
			receive++;
		}
		delay_us(1);
	}
	if (!ack)
	{
		IIC_NAck(_IIC);
	}
	else
	{
		IIC_Ack(_IIC);
	}
	return receive;
}

IIC_Class IIC = {
	.IIC_1 = {
		.GPIO_SCL.GPIO_Pin = GPIO_Pin_8,
		.GPIO_SCL.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SCL.GPIO_OType = GPIO_OType_PP,
		.GPIO_SCL.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SCL.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_SDA.GPIO_Pin = GPIO_Pin_9,
		.GPIO_SDA.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SDA.GPIO_OType = GPIO_OType_PP,
		.GPIO_SDA.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SDA.GPIO_PuPd = GPIO_PuPd_UP,
		.SDA_GPIO_PinSource = GPIO_PinSource9,
		.GPIO_SCL_Port = GPIOB,
		.GPIO_SDA_Port = GPIOB,
		.RCC_APBPeriph_No = RCC_AHB1Periph_GPIOB,
		.RCC_APBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
	},
	.init = init,
	.IIC_Start = _IIC_Start,
	.IIC_Stop = _IIC_Stop,
	.IIC_Send_Byte = IIC_Send_Byte,
	.IIC_Read_Byte = IIC_Read_Byte,
	.IIC_Wait_Ack = IIC_Wait_Ack,
	.IIC_Ack = IIC_Ack,
	.IIC_NAck = IIC_NAck,
};
