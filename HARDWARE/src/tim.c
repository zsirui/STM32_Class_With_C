#include "tim.h"

volatile uint64_t _usticks = 0;

static void initPWM(PWM_IO PWM)
{
	PWM.RCC_AHBPeriphClockCmd(PWM.RCC_AHBPeriph, ENABLE);
	PWM.RCC_APBPeriphClockCmd(PWM.RCC_APBPeriph, ENABLE);
	GPIO_PinAFConfig(PWM.GPIO_Port, PWM.GPIO_PinSource, PWM.GPIO_AF_TIM);
	GPIO_Init(PWM.GPIO_Port, &PWM.GPIO_AF_InitStructure);
	GPIO_Init(PWM.GPIO_Port, &PWM.GPIO_IO_InitStructure);
	TIM_TimeBaseInit(PWM.TIM_PORT, &PWM.TIM_TimeBaseStructure);
	PWM.TIM_OCInit(PWM.TIM_PORT, &PWM.TIM_OCInitStructure);
	PWM.TIM_OCPreloadConfig(PWM.TIM_PORT, PWM.TIM_OCPreloadState);
	TIM_ARRPreloadConfig(PWM.TIM_PORT, PWM.TIM_ARRPreloadState);
	TIM_CCxCmd(PWM.TIM_PORT, PWM.TIM_Channel, TIM_CCx_Disable);
}

static void deinitPWM(PWM_IO PWM)
{
	PWM.RCC_APBPeriphClockCmd(PWM.RCC_APBPeriph, DISABLE);
	PWM.TIM_OCPreloadConfig(PWM.TIM_PORT, TIM_OCPreload_Disable);
	TIM_ARRPreloadConfig(PWM.TIM_PORT, DISABLE);
}

static void initTimer(TIMER_IO _TIMER)
{
	NVIC_Init(&_TIMER.NVIC_InitStructure);
	TIM_DeInit(_TIMER.TIM_PORT);
	_TIMER.RCC_APBPeriphClockCmd(_TIMER.RCC_APBPeriph, ENABLE);
	TIM_TimeBaseInit(_TIMER.TIM_PORT, &_TIMER.TIM_TimeBaseStructure);
	TIM_ClearFlag(_TIMER.TIM_PORT, TIM_FLAG_Update);
	TIM_ITConfig(_TIMER.TIM_PORT, TIM_IT_Update, ENABLE);
	TIM_Cmd(_TIMER.TIM_PORT, ENABLE);
}

static void deinitTimer(TIMER_IO _TIMER)
{
	TIM_DeInit(_TIMER.TIM_PORT);
	TIM_Cmd(_TIMER.TIM_PORT, DISABLE);
}

static void StepMotorInit(PWM_IO STEPMOTOR)
{
	STEPMOTOR.RCC_AHBPeriphClockCmd(STEPMOTOR.RCC_AHBPeriph, ENABLE);
	STEPMOTOR.RCC_APBPeriphClockCmd(STEPMOTOR.RCC_APBPeriph, ENABLE);
	GPIO_PinAFConfig(STEPMOTOR.GPIO_Port, STEPMOTOR.GPIO_PinSource, STEPMOTOR.GPIO_AF_TIM);
	GPIO_Init(STEPMOTOR.GPIO_Port, &STEPMOTOR.GPIO_AF_InitStructure);
	GPIO_Init(STEPMOTOR.GPIO_Port, &STEPMOTOR.GPIO_IO_InitStructure);
}

static void StepMotorConfig(PWM_IO STEPMOTOR, uint32_t Period, uint32_t Prescaler)
{
	STEPMOTOR.TIM_TimeBaseStructure.TIM_Prescaler = Prescaler;
	STEPMOTOR.TIM_TimeBaseStructure.TIM_Period = Period;
	STEPMOTOR.TIM_OCInitStructure.TIM_Pulse = (uint32_t)(Period * 50 / 100);
	TIM_TimeBaseInit(STEPMOTOR.TIM_PORT, &STEPMOTOR.TIM_TimeBaseStructure);
	STEPMOTOR.TIM_OCInit(STEPMOTOR.TIM_PORT, &STEPMOTOR.TIM_OCInitStructure);
	STEPMOTOR.TIM_OCPreloadConfig(STEPMOTOR.TIM_PORT, STEPMOTOR.TIM_OCPreloadState);
	TIM_ARRPreloadConfig(STEPMOTOR.TIM_PORT, STEPMOTOR.TIM_ARRPreloadState);
	TIM_Cmd(STEPMOTOR.TIM_PORT, ENABLE);
}

static uint64_t getus(void)
{
	return _usticks;
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		_usticks++;
	}
}

TIMER_Class TIMER = {
	.PWM_TIM3 = {
		.GPIO_PinSource = GPIO_PinSource6,
		.GPIO_AF_TIM = GPIO_AF_TIM3,
		.TIM_Channel = TIM_Channel_1,
		.RCC_AHBPeriph = RCC_AHB1Periph_GPIOC,
		.RCC_APBPeriph = RCC_APB1Periph_TIM3,
		.TIM_PORT = TIM3,
		.GPIO_Port = GPIOC,
		.GPIO_AF_InitStructure = {
			.GPIO_Pin = GPIO_Pin_6,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
		},
		.GPIO_IO_InitStructure = {
			.GPIO_Pin = GPIO_Pin_7,
			.GPIO_Mode = GPIO_Mode_OUT,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP,
		},
		.TIM_TimeBaseStructure = {
			.TIM_Prescaler = 2,
			.TIM_CounterMode = TIM_CounterMode_Up,
			.TIM_Period = 65521,
			.TIM_RepetitionCounter = 0,
			.TIM_ClockDivision = TIM_CKD_DIV1,
		},
		.TIM_OCInitStructure = {
			.TIM_OCMode = TIM_OCMode_PWM1,
			.TIM_Pulse = 6552,
			.TIM_OutputState = TIM_OutputState_Enable,
			.TIM_OCPolarity = TIM_OCPolarity_High,
		},
		.TIM_ARRPreloadState = ENABLE,
		.TIM_OCPreloadState = TIM_OCPreload_Enable,
		.TIM_OCPreloadConfig = TIM_OC1PreloadConfig,
		.RCC_APBPeriphClockCmd = RCC_APB1PeriphClockCmd,
		.RCC_AHBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
		.TIM_OCInit = TIM_OC1Init,
	},
	.PWM_TIM4 = {
		.GPIO_PinSource = GPIO_PinSource15,
		.GPIO_AF_TIM = GPIO_AF_TIM4,
		.TIM_Channel = TIM_Channel_1,
		.RCC_AHBPeriph = RCC_AHB1Periph_GPIOD,
		.RCC_APBPeriph = RCC_APB1Periph_TIM4,
		.TIM_PORT = TIM4,
		.GPIO_Port = GPIOD,
		.GPIO_AF_InitStructure = {
			.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_14,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP,
		},
		.GPIO_IO_InitStructure = {
			.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12,
			.GPIO_Mode = GPIO_Mode_IN,
			.GPIO_Speed = GPIO_Speed_100MHz,
			.GPIO_OType = GPIO_OType_OD,
			.GPIO_PuPd = GPIO_PuPd_UP,
		},
		.TIM_TimeBaseStructure = {
			.TIM_CounterMode = TIM_CounterMode_Up,
			.TIM_RepetitionCounter = 0,
			.TIM_ClockDivision = TIM_CKD_DIV1,
		},
		.TIM_OCInitStructure = {
			.TIM_OCMode = TIM_OCMode_PWM1,
			.TIM_OutputState = TIM_OutputState_Enable,
			.TIM_OCPolarity = TIM_OCPolarity_Low,
		},
		.TIM_ARRPreloadState = DISABLE,
		.TIM_OCPreloadState = TIM_OCPreload_Disable,
		.TIM_OCPreloadConfig = TIM_OC4PreloadConfig,
		.RCC_APBPeriphClockCmd = RCC_APB1PeriphClockCmd,
		.RCC_AHBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
		.TIM_OCInit = TIM_OC4Init,
	},
	.TIMER_8 = {
		.NVIC_InitStructure = {
			.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn,
			.NVIC_IRQChannelPreemptionPriority = 0,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
		.TIM_TimeBaseStructure = {
			.TIM_Period = 168 - 1,
			.TIM_Prescaler = 0,
			.TIM_ClockDivision = TIM_CKD_DIV1,
			.TIM_CounterMode = TIM_CounterMode_Up,
			.TIM_RepetitionCounter = 0,
		},
		.TIM_PORT = TIM8,
		.RCC_APBPeriphClockCmd = RCC_APB2PeriphClockCmd,
		.RCC_APBPeriph = RCC_APB2Periph_TIM8,
	},
	.getus = getus,
	.initPWM = initPWM,
	.initTimer = initTimer,
	.deinitPWM = deinitPWM,
	.deinitTimer = deinitTimer,
	.StepMotorInit = StepMotorInit,
	.StepMotorConfig = StepMotorConfig,
};
