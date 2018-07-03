#ifndef __DELAY_H
#define __DELAY_H
#include "stdint.h"
#include "stdio.h"
#include "stm32f4xx.h"

extern volatile uint64_t SystemTime;

typedef struct
{
	uint8_t minutes;
	uint8_t seconds;
	uint16_t milliseconds;
	uint32_t hours;
}Time_TypeDef;

void set_board_systick(void);
void delay_ms(uint32_t ms);
void delay_us(volatile uint32_t us);
uint64_t getms(void);
uint64_t getus(void);
void TIM8_Init(void);
void time_data_change(uint64_t ms, Time_TypeDef* Time_Structure);

#endif
