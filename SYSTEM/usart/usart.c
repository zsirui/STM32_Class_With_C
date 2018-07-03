#include "sys.h"
#include "usart.h"	

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR & 0x40) == 0);//循环发送,直到发送完毕   
	USART3->DR = (u8) ch;      
	return ch;
}
#endif

static void send_data(unsigned char data)
{
	USART3->DR = (data & (uint16_t)0x01FF);
	while (!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
}

static void init(uint32_t _bound, uint8_t _rx_pinsource, uint8_t _tx_pinsource, uint16_t _RX_PIN,
				 uint16_t _TX_PIN, GPIO_TypeDef* _GPIO_Port, uint32_t _GPIO_RCC_AHB, uint16_t _WordLength,
				 uint16_t _USART_StopBits, uint16_t _Parity, uint16_t _HardwareFlowControl, uint16_t _Mode,
				 BOOL _Interrupt, uint8_t _PreemptionPriority, uint8_t _SubPriority, FunctionalState _State)
{
    // GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(_GPIO_RCC_AHB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// 串口3对应引脚复用映射
	GPIO_PinAFConfig(_GPIO_Port, _rx_pinsource, GPIO_AF_USART3);
	GPIO_PinAFConfig(_GPIO_Port, _tx_pinsource, GPIO_AF_USART3);

	// USART3端口配置
	GPIO_InitStructure.GPIO_Pin = _RX_PIN | _TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(_GPIO_Port, &GPIO_InitStructure);

    // USART3 初始化设置
	USART_InitStructure.USART_BaudRate = _bound;
	USART_InitStructure.USART_WordLength = _WordLength;
	USART_InitStructure.USART_StopBits = _USART_StopBits;
	USART_InitStructure.USART_Parity = _Parity;
	USART_InitStructure.USART_HardwareFlowControl = _HardwareFlowControl;
	USART_InitStructure.USART_Mode = _Mode;
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART3, ENABLE);
	if (_Interrupt)
	{
		USART_ClearFlag(USART3, USART_FLAG_TC);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

		// Usart3 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = _PreemptionPriority;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = _SubPriority;
		NVIC_InitStructure.NVIC_IRQChannelCmd = _State;
		NVIC_Init(&NVIC_InitStructure);
	}
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
	}
}

USART_Class USART_3 = {
	.BaudRate = 115200,
	.rx_pinsource = GPIO_PinSource10,
	.tx_pinsource = GPIO_PinSource11,
	.RX_PIN = GPIO_Pin_11,
	.TX_PIN = GPIO_Pin_10,
	.GPIO_RCC_AHB = RCC_AHB1Periph_GPIOB,
	.GPIO_Port = GPIOB,
	.WordLength = USART_WordLength_8b,
	.USART_StopBits = USART_StopBits_1,
	.Parity = USART_Parity_No,
	.HardwareFlowControl = USART_HardwareFlowControl_None,
	.Mode = USART_Mode_Rx | USART_Mode_Tx,
	.init = init,
	.Interrupt = True,
	.PreemptionPriority = 1,
	.SubPriority = 3,
	.State = ENABLE,
	.send_data = send_data,
};

void usart3_init(void)
{
	USART_3.init(USART_3.BaudRate, USART_3.rx_pinsource, USART_3.tx_pinsource, USART_3.RX_PIN,
				 USART_3.TX_PIN, USART_3.GPIO_Port, USART_3.GPIO_RCC_AHB, USART_3.WordLength,
				 USART_3.USART_StopBits, USART_3.Parity, USART_3.HardwareFlowControl, USART_3.Mode,
				 USART_3.Interrupt, USART_3.PreemptionPriority, USART_3.SubPriority, USART_3.State);
}
