/*
* @Author: Zheng Sirui
* @Filename: usart.h
* @Email: zsirui@126.com
* @Date:   2018-04-08 15:52:23
* @Last Modified by:   Zheng Sirui
* @Last Modified time: 2018-06-12 16:23:25
*/
#ifndef __USART_H_
#define __USART_H_

#include "stdio.h"
#include "main.h"
#include "stm32f4xx_conf.h"

typedef struct
{
	uint8_t rx_pinsource;
	uint8_t tx_pinsource;
	uint8_t GPIO_AF_No;
	BOOL Interrupt;
	uint32_t GPIO_RCC_AHB;
	uint32_t RCC_APBPeriph_No;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_TypeDef* GPIO_Port;
	USART_TypeDef* USART_Number;
	void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
}USART_IO;

typedef struct
{
	USART_IO USART_1;
	USART_IO USART_2;
	USART_IO USART_3;
	void (*send_data)(USART_TypeDef* usart_number, unsigned char data);
	void (*init)(USART_IO _USART);
	void (*deinit)(USART_IO _USART);
}USART_Class;

extern USART_Class USART;

#endif
