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

static uint32_t enter_critical_section(void)
{
	uint32_t context = __get_PRIMASK();
	cli();
	return context;
}

static void leave_critical_section(uint32_t context)
{
	__set_PRIMASK(context);
}

void SysTick_Handler(void)
{
	SystemTime++;
}

uint64_t getms(void)
{
	return SystemTime;
}

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

void delay_ms(uint32_t ms)
{
	uint32_t targettime = getms() + ms;

	while( getms() < targettime );
}

static void _delay_loop(volatile uint32_t count)
{
	while(count--);
}

void delay_us(volatile uint32_t us)
{
	_delay_loop(us * _ticks_per_us);
}

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

void set_board_systick(void)
{
	SysTick->LOAD = SYSTICK_1MS_TICKS - 1;
	NVIC_SetPriority(SysTick_IRQn, 0);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	softdelay_calibrate();
}

void time_data_change(uint64_t ms, Time_TypeDef* Time_Structure)
{
	Time_Structure->milliseconds = ms % 1000;
	Time_Structure->seconds = (ms / 1000) % 60;
	Time_Structure->minutes = (ms / 1000 / 60) % 60;
	Time_Structure->hours = ms / 1000 / 60 / 60;
}
