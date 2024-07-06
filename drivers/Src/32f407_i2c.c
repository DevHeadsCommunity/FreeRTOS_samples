#include "../Inc/32f407_i2c.h"


//array of ahb facors
uint16_t AHB_PreScaler[9] =  {2, 4, 8, 16, 32, 64, 128, 256, 512};
uint16_t APB1_PreScaler[4] =  {2,4,8,16};


uint32_t RCC_GetPCLK1Value(void){

	uint32_t pClock1, temp, SystemClock;

	//find clock source
	uint8_t clksrc, ahbp, apb1p;
	clksrc = ((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x3); //2 is the clock source bit field

	// which clock
	if(clksrc == 0){
		SystemClock = 16000000;
	} else if(clksrc == 1){
		SystemClock = 8000000;
	} else if(clksrc == 2){
		//pll source calculation
	}

	// AHB prescaler value
	temp = ((RCC->CFGR >> RCC_CFGR_HPRE_Pos) & 0xF);

	if(temp < 8)
	{
		ahbp = 1;
	}else {
		ahbp = AHB_PreScaler[temp - 8];
	}


	// APB1 Prescaler value

	temp = 0;

	temp = ((RCC->CFGR >> RCC_CFGR_PPRE1_Pos) & 0x7);

	if (temp < 4) {
		apb1p = 1;
	} else {
		apb1p = APB1_PreScaler[temp - 4];
	}

	// pclock1 value

	pClock1 = (SystemClock/ahbp) / apb1p;

	return pClock1;

}


/******
 * @fn I2C_Init
 *
 * @brief Initializes a I2C Perpheral
 *
 * @params[pI2Cx] port handle structure
 *
 * @return void
 * @note
 *  */

void I2C_Init(I2C_Handle_t *pI2CHandle){
	uint32_t temp_reg = 0;


	// ack control
	temp_reg |= pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK_Pos;

	pI2CHandle->pI2Cx->CR1 = temp_reg;
	
    //Set Peripheral clock speed
	temp_reg = 0;

	temp_reg |= RCC_GetPCLK1Value() / 1000000;

	pI2CHandle->pI2Cx->CR2 = temp_reg & 0x3F;

	/// Our Address if in Slave Mode
	temp_reg = 0;
	temp_reg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << I2C_OAR1_ADD1_Pos;

	// TODO consider addressing mode
	temp_reg |= (1 << 14); // 14th bit needs to be kept 1 by software

	pI2CHandle->pI2Cx->OAR1 = temp_reg;

	// CCR Register
	uint16_t ccr_value =  0;
	temp_reg = 0;


	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM) {
		// standard mode
		ccr_value = (RCC_GetPCLK1Value() / ( 2  * pI2CHandle->I2C_Config.I2C_SCLSpeed));

		temp_reg |= (ccr_value & 0xFFF);
	} else {
		// fast mode
		//set mode
		temp_reg |=  (1 << I2C_CCR_FS_Pos); // Mode bit

		// duty cycle
		temp_reg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY_Pos); // duty cycle bit

		//ccr setting
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2){
			ccr_value = (RCC_GetPCLK1Value() / ( 3  * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		} else {
			ccr_value = (RCC_GetPCLK1Value() / ( 25  * pI2CHandle->I2C_Config.I2C_SCLSpeed));

		}

		temp_reg |= (ccr_value & 0xFFF);

	}

	pI2CHandle->pI2Cx->CCR = temp_reg;

}


/******
 * @fn I2C_DeInit
 *
 * @brief  Resets a I2C Perpheral
 *
 * @params[pI2Cx] port handle structure
 *
 * @return void
 * @note
 *  */

void I2C_DeInit(I2C_TypeDef *pI2Cx) {
	if (pI2Cx == I2C1) {
        RCC->APB1RSTR |= (1 << RCC_APB1RSTR_I2C1RST_Pos);
        RCC->APB1RSTR &= ~(1 << RCC_APB1RSTR_I2C1RST_Pos);
	} else if (pI2Cx == I2C2) {
		RCC->APB1RSTR |= (1 << RCC_APB1RSTR_I2C2RST_Pos);
        RCC->APB1RSTR &= ~(1 << RCC_APB1RSTR_I2C2RST_Pos);
	} else if (pI2Cx == I2C3) {
		RCC->APB1RSTR |= (1 << RCC_APB1RSTR_I2C3RST_Pos);
        RCC->APB1RSTR &= ~(1 << RCC_APB1RSTR_I2C3RST_Pos);
	}
}

/******
 * @fn I2C_PeripheralControl
 *
 * @brief  Enables or Disables a I2C peripheral
 *
 * @params[pI2Cx] port handle structure
 * @params[EnorDi] Enable or Disable
 *
 * @return void
 * @note
 *  */
void I2C_PeripheralControl(I2C_TypeDef *pI2Cx, uint8_t EnorDi) {
	if (EnorDi == ASSERT) {
		pI2Cx->CR1 |= (1 << I2C_CR1_PE_Pos);
	} else {
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE_Pos);
	}
}