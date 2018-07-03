#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "mlx.h"
#include "adc.h"
#include "iic.h"
#include "tim.h"
#include "gk105.h"

__IO uint64_t System_TimeTicks = 0;

int main(void)
{
	float temp3;
	ADC_DATA adc_data;
	set_board_systick();
	USART.init(USART.USART_1);
	USART.init(USART.USART_2);
	USART.init(USART.USART_3);
	TIMER.initTimer(TIMER.TIMER_8);
//	MLX.init(MLX.MLX_1);
//	MLX.init(MLX.MLX_2);
//	MLX.init(MLX.MLX_3);
	MLX.init(MLX.MLX_4);
	ADC_Type.init(ADC_Type.ADC_1);
	ADC_SoftwareStartConv(ADC_Type.ADC_1.ADC_No);
	ADC_Type.InitADCStructure(&adc_data);
	GK105.init(GK105.GK105_1);
//	IIC.init(IIC.IIC_1);

	while(1)
	{
		if(getms() - System_TimeTicks >= 500)
		{
//			MLX_1.SMBus_ReadTemp_TOBJ(&temp0, MLX.MLX_1);
//			MLX_2.SMBus_ReadTemp_TOBJ(&temp1, MLX.MLX_2);
//			MLX_3.SMBus_ReadTemp_TOBJ(&temp2, MLX.MLX_3);
			MLX.SMBus_ReadTemp_TOBJ(&temp3, MLX.MLX_4);
			printf("temp: %.2f\r\n", temp3);
			ADC_Type.ADCHeartBeat(&adc_data);
			printf("fliter_counter: %d Current_adcvalue: %d Voltage_adcvalue: %d Voltage_value: %.2f Current_value: %.2f\r\n", adc_data.fliter_counter, adc_data.Current_adcvalue, adc_data.Voltage_adcvalue, adc_data.Voltage_value, adc_data.Current_value);
			printf("GK105_1.counter: %lld\r\n", GK105.counter);
			printf("usticks: %lld\r\n", TIMER.getus());
			printf("msticks: %lld\r\n", getms());
			System_TimeTicks = getms();
		}
	}
}
