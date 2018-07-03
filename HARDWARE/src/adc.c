/*
* @Author: Zheng Sirui
* @Filename: adc.c
* @Email: zsirui@126.com
* @Date:   2018-04-08 15:52:23
* @Last Modified by:   Zheng Sirui
* @Last Modified time: 2018-06-12 16:23:25
*/

#include "adc.h"
#include "main.h"
#include "delay.h"

__IO uint16_t ADC_ConvertedValue[] = {0, 0};

static void init(ADC_IO _ADC)
{
	_ADC.RCC_AHBPeriphClockCmd(_ADC.RCC_AHBPeriph, ENABLE);
	_ADC.RCC_APBPeriphClockCmd(_ADC.RCC_APBPeriph, ENABLE);
	GPIO_Init(_ADC.GPIO_Port, &_ADC.GPIO_InitStructure);
	_ADC.RCC_APBPeriphResetCmd(_ADC.RCC_APBPeriph, ENABLE);
	_ADC.RCC_APBPeriphResetCmd(_ADC.RCC_APBPeriph, DISABLE);
	ADC_CommonInit(&_ADC.ADC_CommonInitStructure);
	ADC_Init(_ADC.ADC_No, &_ADC.ADC_InitStructure);
	ADC_RegularChannelConfig(_ADC.ADC_No, _ADC.ADC_Channel[9], 1, _ADC.SampleTime);
	ADC_RegularChannelConfig(_ADC.ADC_No, _ADC.ADC_Channel[14], 2, _ADC.SampleTime);
	ADC_DMARequestAfterLastTransferCmd(_ADC.ADC_No, ENABLE);
	ADC_DMACmd(_ADC.ADC_No, ENABLE);
	ADC_Cmd(_ADC.ADC_No, ENABLE);

	_ADC.RCC_AHBPeriphClockCmd(_ADC.RCC_AHBPeriph_DMA, ENABLE);
	_ADC.DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&ADC3->DR);
	_ADC.DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_ConvertedValue;
	DMA_Init(_ADC.DMA_Stream, &_ADC.DMA_InitStructure);
	DMA_Cmd(_ADC.DMA_Stream, ENABLE);
}

static void deinit(ADC_IO _ADC)
{
	ADC_DMACmd(_ADC.ADC_No, DISABLE);
	ADC_Cmd(_ADC.ADC_No, DISABLE);
}

ADC_Class ADC_Type = {
	.ADC_1 = {
		.DMA_InitStructure = {
			.DMA_Channel = DMA_Channel_2,
			.DMA_DIR = DMA_DIR_PeripheralToMemory,
			.DMA_BufferSize = 2,
			.DMA_PeripheralInc = DMA_PeripheralInc_Disable,
			.DMA_MemoryInc = DMA_MemoryInc_Enable,
			.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
			.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,
			.DMA_Mode = DMA_Mode_Circular,
			.DMA_Priority = DMA_Priority_High,
			.DMA_FIFOMode = DMA_FIFOMode_Disable,
			.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull,
			.DMA_MemoryBurst = DMA_MemoryBurst_Single,
			.DMA_PeripheralBurst = DMA_PeripheralBurst_Single,
		},
		.ADC_InitStructure = {
			.ADC_Resolution = ADC_Resolution_12b,
			.ADC_ScanConvMode = ENABLE,
			.ADC_ContinuousConvMode = ENABLE,
			.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None,
			.ADC_DataAlign = ADC_DataAlign_Right,
			.ADC_NbrOfConversion = 2,
		},
		.ADC_CommonInitStructure = {
			.ADC_Mode = ADC_Mode_Independent,
			.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles,
			.ADC_DMAAccessMode = ADC_DMAAccessMode_1,
			.ADC_Prescaler = ADC_Prescaler_Div4,
		},
		.GPIO_InitStructure = {
			.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3,
			.GPIO_Mode = GPIO_Mode_AN,
			.GPIO_PuPd = GPIO_PuPd_NOPULL,
		},
		.RCC_AHBPeriph = RCC_AHB1Periph_GPIOF,
		.RCC_AHBPeriph_DMA = RCC_AHB1Periph_DMA2,
		.RCC_APBPeriph = RCC_APB2Periph_ADC3,
		.ADC_Channel = {
			0, 1, 2, 3, 4, 5,
			6, 7, 8, 9, 10, 11,
			12, 13, 14, 15, 16, 17
		},
		.GPIO_Port = GPIOF,
		.DMA_Stream = DMA2_Stream0,
		.SampleTime = ADC_SampleTime_480Cycles,
		.ADC_No = ADC3,
		.RCC_APBPeriphClockCmd = RCC_APB2PeriphClockCmd,
		.RCC_AHBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
		.RCC_APBPeriphResetCmd = RCC_APB2PeriphResetCmd,
	},
	.init = init,
	.deinit = deinit,
};
