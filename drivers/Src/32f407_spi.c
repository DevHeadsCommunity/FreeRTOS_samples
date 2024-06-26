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
