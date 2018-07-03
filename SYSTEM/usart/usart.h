#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

typedef struct
{
	uint8_t PreemptionPriority;
	uint8_t SubPriority;
	uint8_t rx_pinsource;
	uint8_t tx_pinsource;
	BOOL Interrupt;
	uint16_t RX_PIN;
	uint16_t TX_PIN;
	uint16_t WordLength;
	uint16_t USART_StopBits;
	uint16_t Parity;
	uint16_t HardwareFlowControl;
	uint16_t Mode;
	uint32_t GPIO_RCC_AHB;
	uint32_t BaudRate;
	FunctionalState State;
	GPIO_TypeDef* GPIO_Port;
	void (*send_data)(unsigned char data);
	void (*init)(uint32_t _bound, uint8_t _rx_pinsource, uint8_t _tx_pinsource, uint16_t _RX_PIN,
				 uint16_t _TX_PIN, GPIO_TypeDef* _GPIO_Port, uint32_t _GPIO_RCC_AHB, uint16_t _WordLength,
				 uint16_t _USART_StopBits, uint16_t _Parity, uint16_t _HardwareFlowControl, uint16_t _Mode,
				 BOOL _Interrupt, uint8_t _PreemptionPriority, uint8_t _SubPriority, FunctionalState _State);
}USART_Class;

void usart3_init(void);
extern USART_Class USART_3;

#endif
