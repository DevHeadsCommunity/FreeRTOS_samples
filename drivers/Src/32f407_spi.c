#include "../Inc/32f407_spi.h"
#include <stdint.h>

/* Clock control for SPI */
void SPI_PCLK_CTRL(SPI_TypeDef *pSPIx, uint8_t EnorDi){
    if(EnorDi == ENABLE) {
        if(pSPIx == SPI1){
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
        } else  if(pSPIx == SPI2){
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
        } else  if(pSPIx == SPI3){
            RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
        } 
    } else {
        if(pSPIx == SPI1){
            RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
        } else  if(pSPIx == SPI2){
            RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
        } else  if(pSPIx == SPI3){
            RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN;
        } 

    }
}

/******
 * @fn SPI_Init
 *
 * @brief  Enables or Disables clock for a GPIO Port
 *
 * @params[pSPIx] port handle structure
 *
 * @return void
 * @note
 *  */

void SPI_Init(SPI_Handle_t *pSPIHandle){
	/*SPI Experiment*/
	SPI_PCLK_CTRL(pSPIHandle->pSPIx,ENABLE);

    //CR1 temp data 
    uint32_t tempreg = 0;

    //set the device mode 
    tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode  << SPI_CR1_MSTR_Pos;

    //bus config
	if(pSPIHandle->SPIConfig.SPI_BUSConfig == SPI_BUS_CONFIG_FD){
		//clear bidimode
		tempreg &= ~(1<< SPI_CR1_BIDIMODE_Pos);
	} else if(pSPIHandle->SPIConfig.SPI_BUSConfig == SPI_BUS_CONFIG_HD){
		//set bidi mode
		tempreg |= (1<< SPI_CR1_BIDIMODE_Pos);

	} else if(pSPIHandle->SPIConfig.SPI_BUSConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY){
		//clear bidi
		tempreg &= ~(1<< SPI_CR1_BIDIMODE_Pos);
		//set rxonly
		tempreg |= (1 << SPI_CR1_RXONLY_Pos);
	}

    //configure clock
	tempreg  |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR_Pos;

	// cofigure frame size
	tempreg  |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF_Pos;

	//cpol
	tempreg  |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL_Pos;

	//CPHA
	tempreg  |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA_Pos;

    //SSM
	tempreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM_Pos;

	pSPIHandle->pSPIx->CR1 = tempreg;

}

void SPI_DeInit(SPI_TypeDef *pSPIx) {
	if (pSPIx == SPI1) {
		RCC->APB2RSTR |= (1 << RCC_APB2RSTR_SPI1RST_Pos);
        RCC->AHB1RSTR &= ~(1 <<  RCC_APB2RSTR_SPI1RST_Pos);
	} else if (pSPIx == SPI2) {
		RCC->APB1RSTR |= (1 << RCC_APB1RSTR_SPI2RST_Pos); 
        RCC->AHB1RSTR &= ~(1 << RCC_APB1RSTR_SPI2RST_Pos);
	} else if (pSPIx == SPI3) {
		RCC->APB1RSTR |= (1 << RCC_APB1RSTR_SPI3RST_Pos); 
        RCC->AHB1RSTR &= ~(1 << RCC_APB1RSTR_SPI3RST_Pos);
	} 
}

uint8_t SPI_SendData(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len){
    //check if SPI is not busy
	uint8_t state = pSPIHandle->TxState;

	if (state != SPI_BUSY_IN_TX) {
		// save txbuffer and len
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;
		//Mark spi as busy in transmission
		pSPIHandle->TxState = SPI_BUSY_IN_TX;
		// ENABLE TXEIE BIT
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE_Pos);
		//transmission now handled in ISR
	}

	return state;
}

uint8_t SPI_ReceiveData(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len){
    //check if SPI is not busy
	uint8_t state = pSPIHandle->RxState;

	if (state != SPI_BUSY_IN_RX) {
		// save txbuffer and len
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;
		//Mark spi as busy in transmission
		pSPIHandle->RxState = SPI_BUSY_IN_RX;
		// ENABLE RXNEIE BIT
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE_Pos);
		//reception now handled in ISR
	}

	return state;
}




/******
 * @fn SPI_SSOEConfig
 *
 * @brief  Enables or Disables a SSOE for a SPI peripheral
 *
 * @params[pSPIx] port handle structure
 * @params[EnorDi] Enable or Disable

 *
 * @return void
 * @note
 *  */
void SPI_SSOEConfig(SPI_TypeDef *pSPIx, uint8_t EnorDi) {
	if (EnorDi == ENABLE) {
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE_Pos);
	} else {
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE_Pos);
	}
}


void SPI_ClearOVRFlag(SPI_Handle_t *pSPIHandle){
	uint8_t temp;

	temp = pSPIHandle->pSPIx->DR;
	temp = pSPIHandle->pSPIx->SR;
	(void) temp;

}
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle){
	//TX done close SPI and tell app
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE_Pos);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
	SPI_ApplicationEventCallBack(pSPIHandle, SPI_EVENT_TX_CMPLT);

}
void SPI_CloseReception(SPI_Handle_t *pSPIHandle){
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE_Pos);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
	SPI_ApplicationEventCallBack(pSPIHandle, SPI_EVENT_RX_CMPLT);
}

//application callback
__attribute((weak)) void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle, uint8_t AppEv){
	// weak implementation for Application to override
}