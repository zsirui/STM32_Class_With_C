#ifndef _TIM_H_
#define _TIM_H_
#include "stm32f4xx_conf.h"
#include "main.h"

typedef struct
{
	uint8_t GPIO_PinSource;
	uint8_t GPIO_AF_TIM;
	uint16_t TIM_Channel;
	uint16_t TIM_OCPreloadState;
	uint32_t RCC_AHBPeriph;
	uint32_t RCC_APBPeriph;
	TIM_TypeDef* TIM_PORT;
	GPIO_InitTypeDef GPIO_AF_InitStructure;
	GPIO_InitTypeDef GPIO_IO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_TypeDef* GPIO_Port;
	FunctionalState TIM_ARRPreloadState;
	void (*TIM_OCPreloadConfig)(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
	void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
	void (*RCC_AHBPeriphClockCmd)(uint32_t RCC_AHBPeriph, FunctionalState NewState);
	void (*TIM_OCInit)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
}PWM_IO;

typedef struct
{
	uint32_t RCC_APBPeriph;
	TIM_TypeDef* TIM_PORT;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
}TIMER_IO;

typedef struct
{
	PWM_IO PWM_TIM3;
	PWM_IO PWM_TIM4;
	TIMER_IO TIMER_8;
	uint64_t (*getus)(void);
	void (*initPWM)(PWM_IO PWM);
	void (*deinitPWM)(PWM_IO PWM);
	void (*initTimer)(TIMER_IO _TIMER);
	void (*deinitTimer)(TIMER_IO _TIMER);
	void (*StepMotorInit)(PWM_IO STEPMOTOR);
	void (*StepMotorConfig)(PWM_IO STEPMOTOR, uint32_t Period, uint32_t Prescaler);
}TIMER_Class;

extern TIMER_Class TIMER;

#endif
