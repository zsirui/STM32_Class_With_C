/*******************************************************************************
* File Name          : delay.h
* Author             : zhengsirui
* Version            : V1.0.3
* Date               : 2017.11.15
* Description        : 系统滴答定时器，延时等
*******************************************************************************/
#ifndef __DELAY_H
#define __DELAY_H
#include <sys.h>
#include <stdio.h>

void _Error_Handler(char *, int);

#define			Error_Handler()			_Error_Handler(__FILE__, __LINE__)
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
void _delay_us(volatile uint64_t us);
uint64_t getms(void);
uint64_t getus(void);
uint64_t getus_timer(void);
void TIM8_Init(void);
void time_data_change(uint64_t ms, Time_TypeDef* Time_Structure);

#endif
