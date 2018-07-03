#include "mlx.h"

//---------------------------------------
//      CALCULATE THE PEC PACKET
//Name: PEC_cal
//Function: Calculate the PEC of received bytes
//Parameters: unsigned char pec[], int n
//Return: pec[0] - This byte contains calculated crc value
//Comments:	Refer to "System Management BUS specification Version 2.0" and " AN "SMBus communication with MLX90614"
//---------------------------------------

static unsigned char PEC_cal(unsigned char pec[], int n)

{
	unsigned char crc[6];
	unsigned char Bitposition = 47;
	unsigned char shift;
	unsigned char i;
	unsigned char j;
	unsigned char temp;
	do
	{
		crc[5] = 0;           			        //Load CRC value 0x000000000107
		crc[4] = 0;
		crc[3] = 0;
		crc[2] = 0;
		crc[1] = 0x01;
		crc[0] = 0x07;
		Bitposition = 47;     		            //Set maximum bit position at 47
		shift = 0;
		//Find first 1 in the transmitted bytes
		i = 5;                					//Set highest index (package byte index)
		j = 0;                			    	//Byte bit index, from lowest
		while((pec[i] & (0x80 >> j)) == 0 && (i > 0))
		{
			Bitposition--;
			if(j < 7)
			{
				j++;
			}
			else
			{
				j = 0x00;
				i--;
			}
		}//End of while, and the position of highest "1" bit in Bitposition is calculated

		shift = Bitposition - 8;                  	//Get shift value for CRC value

		//Shift CRC value left with "shift" bits

		while(shift)
		{
			for(i = 5; i < 0xFF; i--)
			{
				if((crc[i - 1] & 0x80) && (i > 0))    //Check if the MSB of the byte lower is "1"
				{   		                	//Yes - current byte + 1
					temp = 1;						//No - current byte + 0
				}								//So that "1" can shift between bytes
				else
				{
					temp = 0;
				}
				crc[i] <<= 1;
				crc[i] += temp;
			}
			shift--;
		}

		//Exclusive OR between pec and crc

		for(i = 0; i <= 5; i++)
		{
			pec[i] ^= crc[i];
		}

	}while(Bitposition > 8);

	return pec[0];
}

static void SDA_INPUT(GPIO_TypeDef* GPIOx, uint8_t GPIO_PinSource)
{
	GPIOx->MODER &= ~((unsigned int)3 << (GPIO_PinSource * 2));
	GPIOx->MODER |= (0 << (GPIO_PinSource * 2));
}

static void SDA_OUTPUT(GPIO_TypeDef* GPIOx, uint8_t GPIO_PinSource)
{
	GPIOx->MODER &= ~((unsigned int)3 << (GPIO_PinSource * 2));
	GPIOx->MODER |= (1 << (GPIO_PinSource * 2));
}

static void _SMBus_Start(GPIO_TypeDef* SDA_GPIOx, uint8_t SDA_GPIO_PinSource, uint16_t SDA_GPIO_Pin, GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin)
{
	SDA_OUTPUT(SDA_GPIOx, SDA_GPIO_PinSource);
	GPIO_SetBits(SDA_GPIOx, SDA_GPIO_Pin);
	delay_us(1);
	GPIO_SetBits(SCL_GPIOx, SCL_GPIO_Pin);
	delay_us(5);
	GPIO_ResetBits(SDA_GPIOx, SDA_GPIO_Pin);
	delay_us(10);
	GPIO_ResetBits(SCL_GPIOx, SCL_GPIO_Pin);
	delay_us(2);
}

static void _SMBus_Stop(GPIO_TypeDef* SDA_GPIOx, uint8_t SDA_GPIO_PinSource, uint16_t SDA_GPIO_Pin, GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin) //停止信号
{
	SDA_OUTPUT(SDA_GPIOx, SDA_GPIO_PinSource);
	GPIO_ResetBits(SCL_GPIOx, SCL_GPIO_Pin);
	delay_us(5);
	GPIO_ResetBits(SDA_GPIOx, SDA_GPIO_Pin);
	delay_us(5);
	GPIO_SetBits(SCL_GPIOx, SCL_GPIO_Pin);
	delay_us(5);
	GPIO_SetBits(SDA_GPIOx, SDA_GPIO_Pin);
}

static void _SMBus_SendBit(uint8_t bit, GPIO_TypeDef* SDA_GPIOx, uint8_t SDA_GPIO_PinSource, uint16_t SDA_GPIO_Pin, GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin) 		//发送一位数据
{
	SDA_OUTPUT(SDA_GPIOx, SDA_GPIO_PinSource);
	if(bit == 0)
	{
		GPIO_ResetBits(SDA_GPIOx, SDA_GPIO_Pin);
	} else
	{
		GPIO_SetBits(SDA_GPIOx, SDA_GPIO_Pin);
	}
	delay_us(2);
	GPIO_SetBits(SCL_GPIOx, SCL_GPIO_Pin);			//SCL一个上升沿将数据发送出去
	delay_us(10);
	GPIO_ResetBits(SCL_GPIOx, SCL_GPIO_Pin);
	delay_us(10);
}

static uint8_t _SMBus_ReadBit(GPIO_TypeDef* SDA_GPIOx, uint8_t SDA_GPIO_PinSource, uint16_t SDA_GPIO_Pin, GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin)			//接收一位数据
{
	uint8_t REBIT;

	SDA_INPUT(SDA_GPIOx, SDA_GPIO_PinSource);
	GPIO_SetBits(SCL_GPIOx, SCL_GPIO_Pin);
	delay_us(2);

	if(GPIO_ReadInputDataBit(SDA_GPIOx, SDA_GPIO_Pin))
	{
		REBIT = 1;
	}
	else
	{
		REBIT = 0;
	}
	delay_us(2);
	GPIO_ResetBits(SCL_GPIOx, SCL_GPIO_Pin);
	delay_us(3);
	return REBIT;
}

static uint8_t _SMBus_SendByte(uint8_t BYTE, GPIO_TypeDef* SDA_GPIOx, uint8_t SDA_GPIO_PinSource, uint16_t SDA_GPIO_Pin, GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin)		//发送一个字节8位
{
	uint8_t count;
	uint8_t answer;
	for(count = 8; count; count--)
	{
		if(BYTE & 0X80)
		{
			_SMBus_SendBit(1, SDA_GPIOx, SDA_GPIO_PinSource, SDA_GPIO_Pin, SCL_GPIOx, SCL_GPIO_Pin);
		}
		else
		{
			_SMBus_SendBit(0, SDA_GPIOx, SDA_GPIO_PinSource, SDA_GPIO_Pin, SCL_GPIOx, SCL_GPIO_Pin);
		}
		BYTE <<= 1;
	}
	answer = _SMBus_ReadBit(SDA_GPIOx, SDA_GPIO_PinSource, SDA_GPIO_Pin, SCL_GPIOx, SCL_GPIO_Pin);			//读取应答信号  1：错误  0：成功
	return answer;
}

static uint8_t _SMBus_ReadByte(uint8_t answer, GPIO_TypeDef* SDA_GPIOx, uint8_t SDA_GPIO_PinSource, uint16_t SDA_GPIO_Pin, GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin)			//读取一个字节8位
{
	uint8_t RX_BYTE;
	uint8_t count;
	for(count = 8; count; count--)
	{
		if(_SMBus_ReadBit(SDA_GPIOx, SDA_GPIO_PinSource, SDA_GPIO_Pin, SCL_GPIOx, SCL_GPIO_Pin))
		{
			RX_BYTE <<= 1;
			RX_BYTE |= 0x01;
		}
		else
		{
			RX_BYTE <<= 1;
			RX_BYTE &= 0xfe;
		}
	}
	_SMBus_SendBit(answer, SDA_GPIOx, SDA_GPIO_PinSource, SDA_GPIO_Pin, SCL_GPIOx, SCL_GPIO_Pin);
	return RX_BYTE;
}

static uint8_t SMBus_ReadRegister(uint8_t slaveAddress, uint8_t command, uint16_t *data, MLX_IO _MLX)
{
	uint8_t Pec;				// PEC byte storage
	uint8_t DataL = 0;			// Low data byte storage
	uint8_t DataH = 0;			// High data byte storage
	unsigned char arr[6];				//Buffer for the sent bytes
	unsigned char SLA;
	unsigned char PEC;

	SLA = slaveAddress << 1;
	_SMBus_Start(_MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);
	if(_SMBus_SendByte((SLA) | WR, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	if(_SMBus_SendByte(command, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	_SMBus_Start(_MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);
	if(_SMBus_SendByte(SLA | RD, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource,_MLX.GPIO_SDA.GPIO_Pin,  _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	DataL = _SMBus_ReadByte(ACK, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);
	DataH = _SMBus_ReadByte(ACK, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);
	PEC = _SMBus_ReadByte(NACK, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);
	_SMBus_Stop(_MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);

	arr[5] = (SLA);
	arr[4] = command;
	arr[3] = (SLA + 1);
	arr[2] = DataL;
	arr[1] = DataH;
	arr[0] = 0;
	Pec = PEC_cal(arr, 6);  			//Calculate CRC
	if(PEC == Pec)
	{
		*data = (DataH << 8) | DataL;	//data=DataH:DataL
		return 1;
	}
	return 0;
}

static uint8_t SMBus_WriteRegister(uint8_t slaveAddress, uint8_t command, uint16_t data, MLX_IO _MLX)
{
	uint8_t Pec;								// PEC byte storage
	uint8_t DataL = 0;						// Low data byte storage
	uint8_t DataH = 0;						// High data byte storage
	unsigned char arr[6];				//Buffer for the sent bytes
	unsigned char SLA;
	SLA = slaveAddress << 1;
	DataH = data / 256;
	DataL = data % 256;
	arr[5] = (SLA);
	arr[4] = command;
	arr[3] = (SLA + 1);
	arr[2] = DataL;
	arr[1] = DataH;
	arr[0] = 0;
	Pec = PEC_cal(arr, 6);				//Calculate CRC
	_SMBus_Start(_MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);				//Start condition
	if(_SMBus_SendByte((SLA) | WR, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	if(_SMBus_SendByte(command, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	if(_SMBus_SendByte(DataL, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	if(_SMBus_SendByte(DataH, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	if(_SMBus_SendByte(Pec, _MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin))
	{
		return 0;
	}
	_SMBus_Stop(_MLX.GPIO_SDA_Port, _MLX.SDA_GPIO_PinSource, _MLX.GPIO_SDA.GPIO_Pin, _MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);				//停止标志

	return 0;
}

static uint8_t _SMBus_WriteSlaveAddress(uint16_t addr, MLX_IO _MLX)
{
	u8 yes = 0;

	yes = SMBus_WriteRegister(MLX_ADDR, EEPROM | SLAVEADDR, 0x0000, _MLX);
	if(yes)
	{
		yes = SMBus_WriteRegister(0x0000, EEPROM | SLAVEADDR, addr, _MLX);
	}
	return yes;
}

static uint8_t _SMBus_ReadTemp_TOBJ(float *temp, MLX_IO _MLX)//物体温度
{
	u16 data;
	u8 yes = 0;

	yes = SMBus_ReadRegister(MLX_ADDR, RAM | OBJTEMPADDR, &data, _MLX);
	if(yes)
	{
		*temp = data * 0.02 - 273.15;
	}
	return yes;
}

static uint8_t _SMBus_ReadTemp_surr(float *temp, MLX_IO _MLX)//环境温度
{
	u16 data;
	u8 yes = 0;

	yes = SMBus_ReadRegister(MLX_ADDR, RAM | ENVITEMPADDR, &data, _MLX);
	if(yes)
	{
		*temp = data * 0.02 - 273.15;
	}
	return yes;
}

static void init(MLX_IO _MLX)
{
	_MLX.RCC_APBPeriphClockCmd(_MLX.RCC_APBPeriph_No, ENABLE);
	GPIO_Init(_MLX.GPIO_SCL_Port, &_MLX.GPIO_SCL);
	GPIO_Init(_MLX.GPIO_SDA_Port, &_MLX.GPIO_SDA);
	GPIO_SetBits(_MLX.GPIO_SCL_Port, _MLX.GPIO_SCL.GPIO_Pin);
	GPIO_SetBits(_MLX.GPIO_SDA_Port, _MLX.GPIO_SDA.GPIO_Pin);
}

MLX_Class MLX = {
	.MLX_1 = {
		.GPIO_SCL.GPIO_Pin = SCL1,
		.GPIO_SCL.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SCL.GPIO_OType = GPIO_OType_PP,
		.GPIO_SCL.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SCL.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_SDA.GPIO_Pin = SDA1,
		.GPIO_SDA.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SDA.GPIO_OType = GPIO_OType_PP,
		.GPIO_SDA.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SDA.GPIO_PuPd = GPIO_PuPd_UP,
		.SDA_GPIO_PinSource = GPIO_PinSource8,
		.GPIO_SCL_Port = GPIOE,
		.GPIO_SDA_Port = GPIOE,
		.RCC_APBPeriph_No = RCC_AHB1Periph_GPIOE,
		.RCC_APBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
	},
	.MLX_2 = {
		.GPIO_SCL.GPIO_Pin = SCL2,
		.GPIO_SCL.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SCL.GPIO_OType = GPIO_OType_PP,
		.GPIO_SCL.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SCL.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_SDA.GPIO_Pin = SDA2,
		.GPIO_SDA.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SDA.GPIO_OType = GPIO_OType_PP,
		.GPIO_SDA.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SDA.GPIO_PuPd = GPIO_PuPd_UP,
		.SDA_GPIO_PinSource = GPIO_PinSource1,
		.GPIO_SCL_Port = GPIOE,
		.GPIO_SDA_Port = GPIOG,
		.RCC_APBPeriph_No = RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG,
		.RCC_APBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
	},
	.MLX_3 = {
		.GPIO_SCL.GPIO_Pin = SCL3,
		.GPIO_SCL.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SCL.GPIO_OType = GPIO_OType_PP,
		.GPIO_SCL.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SCL.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_SDA.GPIO_Pin = SDA3,
		.GPIO_SDA.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SDA.GPIO_OType = GPIO_OType_PP,
		.GPIO_SDA.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SDA.GPIO_PuPd = GPIO_PuPd_UP,
		.SDA_GPIO_PinSource = GPIO_PinSource15,
		.GPIO_SCL_Port = GPIOG,
		.GPIO_SDA_Port = GPIOF,
		.RCC_APBPeriph_No = RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOF,
		.RCC_APBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
	},
	.MLX_4 = {
		.GPIO_SCL.GPIO_Pin = SCL4,
		.GPIO_SCL.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SCL.GPIO_OType = GPIO_OType_PP,
		.GPIO_SCL.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SCL.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_SDA.GPIO_Pin = SDA4,
		.GPIO_SDA.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_SDA.GPIO_OType = GPIO_OType_PP,
		.GPIO_SDA.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_SDA.GPIO_PuPd = GPIO_PuPd_UP,
		.SDA_GPIO_PinSource = GPIO_PinSource13,
		.GPIO_SCL_Port = GPIOF,
		.GPIO_SDA_Port = GPIOF,
		.RCC_APBPeriph_No = RCC_AHB1Periph_GPIOF,
		.RCC_APBPeriphClockCmd = RCC_AHB1PeriphClockCmd,
	},
	.init = init,
	.SMBus_ReadTemp_TOBJ = _SMBus_ReadTemp_TOBJ,
	.SMBus_ReadTemp_surr = _SMBus_ReadTemp_surr,
	.SMBus_WriteSlaveAddress = _SMBus_WriteSlaveAddress,
};
