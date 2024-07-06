#ifndef B3BA84CC_5247_4A36_93E3_D2E2F073F616
#define B3BA84CC_5247_4A36_93E3_D2E2F073F616

#include "../../CMSIS/Inc/stm32f407xx.h"


typedef struct {
	uint32_t I2C_SCLSpeed;
	uint8_t  I2C_DeviceAddress;
	uint8_t  I2C_ACKControl;
	uint16_t I2C_FMDutyCycle;
}I2C_Config_t;


//Handle Struct

typedef struct {
	I2C_TypeDef *pI2Cx;
	I2C_Config_t I2C_Config;
}I2C_Handle_t;


//FM DUTY CYCLE
#define I2C_FM_DUTY_2		0
#define I2C_FM_DUTY_16_9	1


void FromI2C();

#endif /* B3BA84CC_5247_4A36_93E3_D2E2F073F616 */
