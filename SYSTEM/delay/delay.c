/*******************************************************************************
* File Name          : delay.h
* Author             : zhengsirui
* Version            : V1.1
* Date               : 2017.12.19
* Description        : 系统滴答定时器，延时等
*******************************************************************************/
#include "delay.h"

#define			CPU_FREQ				167999000L
#define 		SYSTICK_1MS_TICKS    	(CPU_FREQ / 1000)
#define			SYSTICK_uS_PER_TICK		1000L / SYSTICK_1MS_TICKS
#define 		CALIBRATION_TICKS 		50000UL
#define 		cli 					__disable_irq
#define 		sei 					__enable_irq
uint32_t _ticks_per_us = 168;
volatile uint64_t usticks = 0;
volatile uint64_t SystemTime = 0;
Time_TypeDef Time_DefStruct;

/******************************************************************
* Function Name  : enter_critical_section
* Description    : 进入临界保护
* Input          :
* Output         :
* Return         : 返回PRIMASK寄存器值
********************************************************************/
static uint32_t enter_critical_section(void)
{
	uint32_t context = __get_PRIMASK();
	cli();
	return context;
}

/******************************************************************
* Function Name  : enter_critical_section
* Description    : 离开临界保护
* Input          :
* Output         :
* Return         :
********************************************************************/
static void leave_critical_section(uint32_t context)
{
	__set_PRIMASK(context);
}

/******************************************************************
* Function Name  : SysTick_Handler
* Description    : 滴答计时器中断
* Input          :
* Output         :
* Return         :
********************************************************************/
void SysTick_Handler(void)
{
	SystemTime++;
}

/******************************************************************
* Function Name  : getms
* Description    : 获取毫秒计数
* Input          :
* Output         :
* Return         : 返回自系统启动后毫秒计数值
********************************************************************/
uint64_t getms(void)
{
	return SystemTime;
}

/******************************************************************
* Function Name  : getus
* Description    : 获取微秒计数
* Input          :
* Output         :
* Return         : 返回微秒计数值
********************************************************************/
uint64_t getus(void)
{
	register uint32_t cached_ms, cached_tick;

	uint32_t context = enter_critical_section();

	cached_ms = SystemTime;
	cached_tick = SysTick->VAL;

	if (SCB->ICSR & SCB_ICSR_PENDSTSET_Msk) {
		++cached_ms;
		cached_tick = SysTick->VAL;
	}

	leave_critical_section(context);

	return (uint64_t)cached_ms * 1000 + ((SYSTICK_1MS_TICKS - 1 - cached_tick) * SYSTICK_uS_PER_TICK);

}

uint64_t getus_timer(void)
{
	return usticks;
}

/******************************************************************
* Function Name  : delay_ms
* Description    : 精确毫秒延时
* Input          : ms 延时毫秒值
* Output         :
* Return         :
********************************************************************/
void delay_ms(uint32_t ms)
{
	uint32_t targettime = getms() + ms;

	while( getms() < targettime );
}

/******************************************************************
* Function Name  : _delay_loop
* Description    : 微秒延时空指令循环
* Input          : count 计数值
* Output         :
* Return         :
********************************************************************/
static void _delay_loop(volatile uint32_t count)
{
	while(count--);
}

/******************************************************************
* Function Name  : delay_us
* Description    : 微秒延时
* Input          : us 延时微秒数
* Output         :
* Return         :
********************************************************************/
void delay_us(volatile uint32_t us)
{
	_delay_loop(us * _ticks_per_us);
}

void _delay_us(volatile uint64_t us)
{
    uint64_t targettime = getus_timer() + us;

    while( getus_timer() < targettime );
}

/******************************************************************
* Function Name  : softdelay_calibrate
* Description    : 微秒延时校准
* Input          :
* Output         :
* Return         :
********************************************************************/
void softdelay_calibrate(void)
{
	uint64_t usedTime;
	uint64_t startUs = getus();
	_delay_loop(CALIBRATION_TICKS);
	usedTime = getus() - startUs;

	if (!usedTime) usedTime = 1;

	_ticks_per_us = CALIBRATION_TICKS / usedTime;
	if (!_ticks_per_us) _ticks_per_us = 1;
}

/******************************************************************
* Function Name  : set_board_systick
* Description    : 设置系统滴答计时器参数
* Input          :
* Output         :
* Return         :
********************************************************************/
void set_board_systick(void)
{
	SysTick->LOAD = SYSTICK_1MS_TICKS - 1;
	NVIC_SetPriority(SysTick_IRQn, 0);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	softdelay_calibrate();
}

/******************************************************************
* Function Name  : time_data_change
* Description    : 时间戳转时间结构体
* Input          : ms 毫秒时间戳，Time_Structure 时间结构体指针
* Output         :
* Return         :
********************************************************************/
void time_data_change(uint64_t ms, Time_TypeDef* Time_Structure)
{
	Time_Structure->milliseconds = ms % 1000;
	Time_Structure->seconds = (ms / 1000) % 60;
	Time_Structure->minutes = (ms / 1000 / 60) % 60;
	Time_Structure->hours = ms / 1000 / 60 / 60;
}

 void TIM8_Init(void)
 {
 	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

 	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
 	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);

 	TIM_DeInit(TIM8);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
 	TIM_TimeBaseStructure.TIM_Period = 168 - 1;
 	TIM_TimeBaseStructure.TIM_Prescaler = 0;
 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
 	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
 	TIM_ClearFlag(TIM8, TIM_FLAG_Update);
 	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
 	TIM_Cmd(TIM8, ENABLE);
 }

 void TIM8_UP_TIM13_IRQHandler(void)
 {
 	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
 	{
 		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
 		usticks++;
 	}
 }
