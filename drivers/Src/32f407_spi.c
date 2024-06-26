#include "../Inc/32f407_spi.h"

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