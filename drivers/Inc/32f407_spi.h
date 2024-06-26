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


// Device Modes
#define SPI_DEVICE_MODE_MASTER	1
#define SPI_DEVICE_MODE_SLAVE 	0
// SPI Bus Modes
#define SPI_BUS_CONFIG_FD				1
#define SPI_BUS_CONFIG_HD				2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY	3
// SPI CLock SPeed
#define SPI_SCLK_SPEED_DIV2					0
#define SPI_SCLK_SPEED_DIV4					1
#define SPI_SCLK_SPEED_DIV8					2
#define SPI_SCLK_SPEED_DIV16				3
#define SPI_SCLK_SPEED_DIV32				4
#define SPI_SCLK_SPEED_DIV264				5
#define SPI_SCLK_SPEED_DIV64				5
#define SPI_SCLK_SPEED_DIV128				6
#define SPI_SCLK_SPEED_DIV256				7

//SPI DFF
#define SPI_DFF_8BITS	0
#define SPI_DFF_16BITS	1
//SPI CPOL
#define SPI_CPOL_HIGH	1
#define SPI_CPOL_LOW	0
//SPI CPHA
#define SPI_CPHA_HIGH	1
#define SPI_CPHA_LOW	0
//SPI SSM
#define SPI_SSM_EN		1
#define SPI_SSM_DI		0


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