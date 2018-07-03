#include "gk105.h"
#include "delay.h"

static void init(GK105_IO _GK105)
{
	_GK105.RCC_APBPeriphClockCmd(_GK105.RCC_APBPeriph, ENABLE);
	_GK105.RCC_AHBPeriphClockCmd(_GK105.RCC_AHBPeriph, ENABLE);
	GPIO_Init(_GK105.GPIO_Port, &_GK105.GPIO_InitStructure);
	_GK105.EXTILineConfig(_GK105.EXTI_PortSource, _GK105.EXTI_PinSource);
	EXTI_ClearITPendingBit(_GK105.EXTI_Line);
	EXTI_Init(&_GK105.EXTI_InitStructure);
	NVIC_Init(&_GK105.NVIC_InitStructure);
}

static void deinit(GK105_IO _GK105)
{
}

static float calcTurnSpeed(uint64_t counter, uint64_t last_counter, uint64_t timer_ticks)
{
	float speed = 0.0;
	while(getms() - timer_ticks >= 1000)
	{
		speed = (float)((counter - last_counter) / 2.0f / 50.0f / ((getms() - timer_ticks) / 1000.0f));
		last_counter = counter;
		timer_ticks = getms();
	}
	return speed;
}

GK105_Class GK105 = {
	.GK105_1 = {
		.GPIO_InitStructure = {
			.GPIO_Pin = GPIO_Pin_1,
			.GPIO_Mode = GPIO_Mode_IN,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_PuPd = GPIO_PuPd_UP, 
		},
		.EXTI_InitStructure = {
			.EXTI_Line = EXTI_Line1,
			.EXTI_Mode = EXTI_Mode_Interrupt,
			.EXTI_Trigger = EXTI_Trigger_Rising_Falling,
			.EXTI_LineCmd = ENABLE,
		},
		.NVIC_InitStructure = {
			.NVIC_IRQChannel = EXTI1_IRQn,
			.NVIC_IRQChannelPreemptionPriority = 0,
			.NVIC_IRQChannelSubPriority = 1,
			.NVIC_IRQChannelCmd = ENABLE,
		},
		.GPIO_Port = GPIOB,
		.EXTI_Line = EXTI_Line1,
		.EXTI_PortSource = EXTI_PortSourceGPIOB,
		.EXTI_PinSource = EXTI_PinSource1,
		.RCC_APBPeriph = RCC_APB2Periph_SYSCFG,
		.RCC_AHBPeriph = RCC_AHB1Periph_GPIOB,
		.RCC_APBPeriphClockCmd = RCC_APB2PeriphClockCmd,
		.RCC_AHBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
		.EXTILineConfig = SYSCFG_EXTILineConfig,
	},
	.init = init,
	.deinit = deinit,
	.calcTurnSpeed = calcTurnSpeed,
	.counter = 0,
	.last_counter = 0,
	.timer_ticks = 0,
};

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(GK105.GK105_1.EXTI_Line) != RESET)
	{
		EXTI_ClearITPendingBit(GK105.GK105_1.EXTI_Line);
		GK105.counter++;
	}
}
