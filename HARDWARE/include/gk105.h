#ifndef __GK105_H_
#define __GK105_H_

#include "stm32f4xx_conf.h"

typedef struct
{
	uint8_t EXTI_PinSource;
	uint8_t EXTI_PortSource;
	uint32_t RCC_AHBPeriph;
	uint32_t RCC_APBPeriph;
	uint32_t EXTI_Line;
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_TypeDef* GPIO_Port;
	void (*NVIC_Init)(NVIC_InitTypeDef* NVIC_InitStruct);
	void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
	void (*RCC_AHBPeriphClockCmd)(uint32_t RCC_AHBPeriph, FunctionalState NewState);
	void (*EXTILineConfig)(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex);
}GK105_IO;

typedef struct
{
	GK105_IO GK105_1;
	uint64_t timer_ticks;
	uint64_t counter;
	uint64_t last_counter;
	void (*init)(GK105_IO _GK105);
	void (*deinit)(GK105_IO _GK105);
	float (*calcTurnSpeed)(uint64_t counter, uint64_t last_counter, uint64_t timer_ticks);
}GK105_Class;

extern GK105_Class GK105;

#endif
