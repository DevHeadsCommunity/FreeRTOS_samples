#include "../Inc/32f407_i2c.h"
#include <stdio.h>


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


static void  I2C_GenerateStartCondition(I2C_TypeDef *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);


static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle );
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle );






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
	I2C_PCLK_CTRL(pI2CHandle->pI2Cx, ASSERT);

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

	// Handle TRISE config
	if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM) {
		//standard Mode
		temp_reg = (RCC_GetPCLK1Value() / 1000000U) + 1;

	} else {
		//fast mode
		temp_reg = ((RCC_GetPCLK1Value() * 300) / 1000000U) + 1;

	}

	pI2CHandle->pI2Cx->TRISE = temp_reg & 0x3F;

	// Enable Peripheral for use now
	I2C_PeripheralControl(pI2CHandle->pI2Cx, ASSERT);

	// Enable Acking 
	I2C_ManageAcking(pI2CHandle->pI2Cx, ASSERT);


	//Enable Interrupts only non blocking for now
	if(pI2CHandle->pI2Cx == I2C1){
		NVIC_EnableIRQ(I2C1_EV_IRQn);
		NVIC_EnableIRQ(I2C1_ER_IRQn);
	} else if(pI2CHandle->pI2Cx == I2C2){
		NVIC_EnableIRQ(I2C2_EV_IRQn);
		NVIC_EnableIRQ(I2C2_ER_IRQn);
	} else if(pI2CHandle->pI2Cx == I2C3){
		NVIC_EnableIRQ(I2C3_EV_IRQn);
		NVIC_EnableIRQ(I2C3_ER_IRQn);
	}

	

}

/******
 * @fn I2C_PCLK_CTRL
 *
 * @brief  Enables the clock Disables a I2C peripheral clock
 *
 * @params[pI2Cx] port handle structure
 * @params[EnorDi] Enable or Disable

 *
 * @return void
 * @note
 *  */
void I2C_PCLK_CTRL(I2C_TypeDef *pI2Cx, uint8_t EnorDi){
	if(EnorDi == ASSERT){
		if(pI2Cx == I2C1){
			RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C1EN_Pos);
		} else if(pI2Cx == I2C2){
			RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C2EN_Pos);
		} else {
			RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C2EN_Pos);
		}
	} else {
		if (pI2Cx == I2C1) {
			RCC->APB1ENR &= ~(1 << RCC_APB1ENR_I2C1EN_Pos);

		} else if (pI2Cx == I2C2) {
			RCC->APB1ENR &= ~(1 << RCC_APB1ENR_I2C1EN_Pos);

		} else {
			RCC->APB1ENR &= ~(1 << RCC_APB1ENR_I2C1EN_Pos);

		}
	}
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

uint8_t I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Generate START Condition
        I2C_GenerateStartCondition(pI2CHandle->pI2Cx);
	
		//enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN_Pos);

		//enable ITEVFEN Control Bit
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN_Pos);
		
		//enable ITERREN Control Bit
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN_Pos);
		
	}

	return busystate;

}
uint8_t I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr){
    uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN_Pos);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN_Pos);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN_Pos);

		//Generate Start condition after enabling interrupts
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);
	}

	return busystate;
}


void I2C_ManageAcking(I2C_TypeDef *pI2Cx, uint8_t EnorDi){
    if (EnorDi) {
		//Enable Ack
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK_Pos);

	} else {
		//Disable Ack
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK_Pos);

		//wait for rxne
		while(!(pI2Cx->SR1 & (1 << I2C_SR1_RXNE_Pos)))

		// stop condition
        I2C_GenerateStopCondition(pI2Cx);
	}

}
void I2C_GenerateStopCondition(I2C_TypeDef *pI2Cx)
{
	pI2Cx->CR1 |= ( 1 << I2C_CR1_STOP_Pos);
}


static void I2C_GenerateStartCondition(I2C_TypeDef *pI2Cx)
{
	pI2Cx->CR1 |= ( 1 << I2C_CR1_START_Pos);
}



static void I2C_ExecuteAddressPhaseWrite(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr)
{	
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1); //SlaveAddr is Slave address + r/nw bit=0
	pI2Cx->DR = SlaveAddr;
}


static void I2C_ExecuteAddressPhaseRead(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1; //SlaveAddr is Slave address + r/nw bit=1
	printf("After Shift in RX %#X \n", SlaveAddr);

	pI2Cx->DR = SlaveAddr;
}


static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle )
{
	uint32_t dummy_read;
	//check for device mode
	if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL_Pos))
	{
		//device is in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize  == 1)
			{
				//first disable the ack
				I2C_ManageAcking(pI2CHandle->pI2Cx,REFUTE);

				//clear the ADDR flag ( read SR1 , read SR2)
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}

		}
		else
		{
			//clear the ADDR flag ( read SR1 , read SR2)
			dummy_read = pI2CHandle->pI2Cx->SR1;
			dummy_read = pI2CHandle->pI2Cx->SR2;
			(void)dummy_read;

		}

	}
	else
	{
		//device is in slave mode
		//clear the ADDR flag ( read SR1 , read SR2)
		dummy_read = pI2CHandle->pI2Cx->SR1;
		dummy_read = pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;
	}


}


static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle )
{

	if(pI2CHandle->TxLen > 0)
	{
		//1. load the data in to DR
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		//2. decrement the TxLen
		pI2CHandle->TxLen--;

		//3. Increment the buffer address
		pI2CHandle->pTxBuffer++;

	}

}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle )
{
	//do the data reception
	if(pI2CHandle->RxSize == 1)
	{
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->RxLen--;

	}


	if(pI2CHandle->RxSize > 1)
	{
		if(pI2CHandle->RxLen == 2)
		{
			//clear the ack bit
			I2C_ManageAcking(pI2CHandle->pI2Cx,REFUTE);
		}

			//read DR
			*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxLen == 0 )
	{
		//close the I2C data reception and notify the application
		//1. generate the stop condition if repeated start is not allowed
		if(pI2CHandle->Sr == REFUTE)
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//2 . Close the I2C rx
		I2C_CloseReceiveData(pI2CHandle);

		//3. Notify the application
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_RX_CMPLT);
	}
}


void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	//disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN_Pos);

	//disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN_Pos);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.I2C_ACKControl == ASSERT)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx,ASSERT);
	}

}

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN_Pos);

	//Implement the code to disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN_Pos);


	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
}

//IRQ Handling APIs
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle){
	uint32_t EVENT, BUFFER , temp1;
	EVENT = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN_Pos);
	BUFFER = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN_Pos);

	/*
	*	Handle SB event
		Since it only happens in Master mode DevAddress will be the slave
	*/
	temp1 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB_Pos);
	if(EVENT && temp1)
	{
		printf("Handled start \n");
		//read SR1 and write DR
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		} else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}

	/**
	 * @brief Handle the Addressing Event
	 *  - When in Master we send the address 
	 *	- When in Slave we attempt to match our own address 
	 */	
	temp1 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR_Pos);
	if(EVENT && temp1)
	{
		printf("yes ADDR work is done \n");
		// Handle ADDR
		I2C_ClearADDRFlag(pI2CHandle);
	}

	//Handle BTF 
	temp1 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF_Pos);
	
	if(EVENT && temp1)
	{
		//printf("Handled BTF \n");

		// Handle TX mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE_Pos)){
				if(pI2CHandle->TxLen == 0) {
					if(pI2CHandle->Sr == REFUTE) 
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					
					//close tx
					I2C_CloseSendData(pI2CHandle);

					//notify app
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
				
			}
		}

		//Handle RX mode 
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
			// Nothing to do
		}

	}
	// Handle Stop
	temp1 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF_Pos);
	if(EVENT && temp1)
	{
		// TODO Not used in Master 

	}
	//TXE
	temp1 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE_Pos);
	if(EVENT && BUFFER && temp1)
	{
		printf("Handled start TXE \n");

		// proceed if master
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL_Pos)){
			if(pI2CHandle->TxRxState ==  I2C_BUSY_IN_TX){
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}
		

	}
	//RXNE
	temp1 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE_Pos);
	if(EVENT && BUFFER && temp1)
	{
		printf("Handled start RXNE \n");

		//check if master
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL_Pos)){
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}

	}

	

	
}

/**
 * @brief  Handle i2c errors
 * 
 * @param pI2CHandle 
 */
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle) {


	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN_Pos);


/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR_Pos);
	if(temp1  && temp2 )
	{
		printf("This is Bus error \n");

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR_Pos);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO_Pos);
	if(temp1  && temp2)
	{
		printf("This is arbitration lost error \n");

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO_Pos);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);

	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF_Pos);
	if(temp1  && temp2)
	{
		printf("This is ACK failure error \n");

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF_Pos);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR_Pos);
	if(temp1  && temp2)
	{
		printf("This is Overrun/underrun \n");

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR_Pos);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT_Pos);
	if(temp1  && temp2)
	{
		printf("This is Time out error \n");

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT_Pos);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}

}