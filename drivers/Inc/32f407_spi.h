#ifndef INC_32f407_SPI_H_
#define INC_32f407_SPI_H_

#include "../../CMSIS/Inc/main.h"


// Configuration struct for SPIx Peripheral
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BUSConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;

}SPI_Config_t;


//Handle struct for SPIx Peripheral
typedef struct
{
	SPI_TypeDef *pSPIx;
	SPI_Config_t SPIConfig;
}SPI_Handle_t;

//APIs
// Init and DeInit
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_TypeDef *pSPIx);

//ClockSetup
void SPI_PCLK_CTRL(SPI_TypeDef *pSPIx, uint8_t EnorDi);

// Data TX and RX
void SPI_SendData(SPI_TypeDef *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_TypeDef *pSPIx, uint8_t *pRxBuffer, uint32_t Len);


//IRQ config and ISR handling
void SPI_IRQConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

#endif