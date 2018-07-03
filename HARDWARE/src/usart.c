/*
* @Author: Zheng Sirui
* @Filename: usart.c
* @Email: zsirui@126.com
* @Date:   2018-04-08 15:52:23
* @Last Modified by:   Zheng Sirui
* @Last Modified time: 2018-06-12 16:23:25
*/
#include "usart.h"

#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
	while((USART2->SR & 0x40) == 0);// 循环发送,直到发送完毕
	USART2->DR = (u8) ch;
	return ch;
}
#endif

/******************************************************************
* Function Name  : USART1_IRQHandler
* Description    : 串口1中断服务程序
* Input          :
* Output         :
* Return         :
********************************************************************/
void USART1_IRQHandler(void)										// 串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
	}
}

/******************************************************************
* Function Name  : USART2_IRQHandler
* Description    : 串口2中断服务程序
* Input          :
* Output         :
* Return         :
********************************************************************/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
	}
}

/******************************************************************
* Function Name  : USART3_IRQHandler
* Description    : 串口3中断服务程序
* Input          :
* Output         :
* Return         :
********************************************************************/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
	}
}

static void send_data(USART_TypeDef* usart_number, unsigned char data)
{
	usart_number->DR = (data & (uint16_t)0x01FF);
	while (!USART_GetFlagStatus(usart_number, USART_FLAG_TXE));
}

static void init(USART_IO _USART)
{
    // GPIO端口设置
	RCC_AHB1PeriphClockCmd(_USART.GPIO_RCC_AHB, ENABLE);
	_USART.RCC_APBPeriphClockCmd(_USART.RCC_APBPeriph_No, ENABLE);

	// 串口对应引脚复用映射
	GPIO_PinAFConfig(_USART.GPIO_Port, _USART.rx_pinsource, _USART.GPIO_AF_No);
	GPIO_PinAFConfig(_USART.GPIO_Port, _USART.tx_pinsource, _USART.GPIO_AF_No);

	// USART端口配置
	GPIO_Init(_USART.GPIO_Port, &_USART.GPIO_InitStructure);

    // USART 初始化设置
	USART_Init(_USART.USART_Number, &_USART.USART_InitStructure);

	USART_Cmd(_USART.USART_Number, ENABLE);
	if (_USART.Interrupt)
	{
		USART_ClearFlag(_USART.USART_Number, USART_FLAG_TC);
		USART_ITConfig(_USART.USART_Number, USART_IT_RXNE, ENABLE);

		// USART NVIC 配置
		NVIC_Init(&_USART.NVIC_InitStructure);
	}
}

static void deinit(USART_IO _USART)
{
	_USART.RCC_APBPeriphClockCmd(_USART.RCC_APBPeriph_No, DISABLE);
	USART_Cmd(_USART.USART_Number, DISABLE);
}

USART_Class USART = {
	.USART_1 = {
		.GPIO_InitStructure = {
			.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP,
		},
		.USART_InitStructure = {
			.USART_BaudRate = 115200,
			.USART_WordLength = USART_WordLength_8b,
			.USART_StopBits = USART_StopBits_1,
			.USART_Parity = USART_Parity_No,
			.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
			.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
		.NVIC_InitStructure = {
			.NVIC_IRQChannel = USART1_IRQn,
			.NVIC_IRQChannelPreemptionPriority = 1,
			.NVIC_IRQChannelSubPriority = 1,
			.NVIC_IRQChannelCmd = ENABLE,
		},
		.rx_pinsource = GPIO_PinSource7,
		.tx_pinsource = GPIO_PinSource6,
		.GPIO_RCC_AHB = RCC_AHB1Periph_GPIOB,
		.GPIO_Port = GPIOB,
		.USART_Number = USART1,
		.GPIO_AF_No = GPIO_AF_USART1,
		.RCC_APBPeriph_No = RCC_APB2Periph_USART1,
		.Interrupt = True,
		.RCC_APBPeriphClockCmd = RCC_APB2PeriphClockCmd,
	},
	.USART_2 = {
		.GPIO_InitStructure = {
			.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP,
		},
		.USART_InitStructure = {
			.USART_BaudRate = 115200,
			.USART_WordLength = USART_WordLength_8b,
			.USART_StopBits = USART_StopBits_1,
			.USART_Parity = USART_Parity_No,
			.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
			.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
		.NVIC_InitStructure = {
			.NVIC_IRQChannel = USART2_IRQn,
			.NVIC_IRQChannelPreemptionPriority = 1,
			.NVIC_IRQChannelSubPriority = 2,
			.NVIC_IRQChannelCmd = ENABLE,
		},
		.rx_pinsource = GPIO_PinSource6,
		.tx_pinsource = GPIO_PinSource5,
		.GPIO_RCC_AHB = RCC_AHB1Periph_GPIOD,
		.GPIO_Port = GPIOD,
		.USART_Number = USART2,
		.GPIO_AF_No = GPIO_AF_USART2,
		.RCC_APBPeriph_No = RCC_APB1Periph_USART2,
		.Interrupt = True,
		.RCC_APBPeriphClockCmd = RCC_APB1PeriphClockCmd,
	},
	.USART_3 = {
		.GPIO_InitStructure = {
			.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10,
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_UP,
		},
		.USART_InitStructure = {
			.USART_BaudRate = 115200,
			.USART_WordLength = USART_WordLength_8b,
			.USART_StopBits = USART_StopBits_1,
			.USART_Parity = USART_Parity_No,
			.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
			.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
		.NVIC_InitStructure = {
			.NVIC_IRQChannel = USART3_IRQn,
			.NVIC_IRQChannelPreemptionPriority = 1,
			.NVIC_IRQChannelSubPriority = 3,
			.NVIC_IRQChannelCmd = ENABLE,
		},
		.rx_pinsource = GPIO_PinSource11,
		.tx_pinsource = GPIO_PinSource10,
		.GPIO_RCC_AHB = RCC_AHB1Periph_GPIOB,
		.GPIO_Port = GPIOB,
		.USART_Number = USART3,
		.GPIO_AF_No = GPIO_AF_USART3,
		.RCC_APBPeriph_No = RCC_APB1Periph_USART3,
		.Interrupt = True,
		.RCC_APBPeriphClockCmd = RCC_APB1PeriphClockCmd,
	},
	.init = init,
	.deinit = deinit,
	.send_data = send_data,
};
