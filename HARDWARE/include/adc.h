/*
* @Author: Zheng Sirui
* @Filename: adc.h
* @Email: zsirui@126.com
* @Date:   2018-04-08 15:52:23
* @Last Modified by:   Zheng Sirui
* @Last Modified time: 2018-06-12 16:23:25
*/

#ifndef __ADC_H_
#define __ADC_H_

#include "stm32f4xx_conf.h"

#define 	BATTERY_WARNING		30000							//��ر�����ѹ��30000mV��30V��
//ADC���ݽṹ��
typedef struct {
	uint16_t Current_adcvalue;							//����ADC�ɼ�ֵ
	uint16_t Current_adcvalue_max;						//����ADC�ɼ�ֵ���ֵ
	uint16_t Current_adcvalue_min;						//����ADC�ɼ�ֵ���ֵ
	uint16_t Voltage_adcvalue;							//��ѹADC�ɼ�ֵ
	uint16_t Voltage_adcvalue_max;						//��ѹADC�ɼ�ֵ���ֵ
	uint16_t Voltage_adcvalue_min;						//��ѹADC�ɼ�ֵ��Сֵ
	int8_t fliter_counter;								//�˲�������
	float Voltage_value;								//��ѹֵ
	float Current_value;								//����ֵ
}ADC_DATA;

typedef struct
{
	uint8_t ADC_Channel[18];
	uint8_t SampleTime;
	uint32_t RCC_AHBPeriph;
	uint32_t RCC_AHBPeriph_DMA;
	uint32_t RCC_APBPeriph;
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_Stream_TypeDef* DMA_Stream;
	GPIO_TypeDef* GPIO_Port;
	ADC_TypeDef* ADC_No;
	void(*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
	void(*RCC_AHBPeriphClockCmd)(uint32_t RCC_AHBPeriph, FunctionalState NewState);
	void(*RCC_APBPeriphResetCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);
}ADC_IO;

typedef struct
{
	ADC_IO ADC_1;
	void (*init)(ADC_IO _ADC);
	void (*deinit)(ADC_IO _ADC);
	unsigned int (*CalcVoltageMV)(unsigned int adc_value);
	void (*InitADCStructure)(ADC_DATA *data);
	void (*ADCHeartBeat)(ADC_DATA *data);
}ADC_Class;

extern ADC_Class ADC_Type;

#endif
