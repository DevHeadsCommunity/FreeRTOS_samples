#ifndef B3BA84CC_5247_4A36_93E3_D2E2F073F616
#define B3BA84CC_5247_4A36_93E3_D2E2F073F616

#include "../../CMSIS/Inc/stm32f407xx.h"
#include <stdint.h>

//APPLICATION STATES 
#define I2C_READY			0
#define I2C_BUSY_IN_RX		1
#define I2C_BUSY_IN_TX		2




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
	uint8_t *pTxBuffer;
	uint8_t *pRxBuffer;
	uint32_t RxLen;
	uint32_t TxLen;
	uint8_t TxRxState;
	uint8_t DevAddr;
	uint32_t RxSize;
	/* Repeated Start Generation*/
	uint8_t Sr;
}I2C_Handle_t;


//FM DUTY CYCLE
#define I2C_FM_DUTY_2		0
#define I2C_FM_DUTY_16_9	1

//SPEEDS
#define I2C_SCL_SPEED_SM 100000
#define I2C_SCL_SPEED_FM 400000

/*
 * I2C application events macros
 */
#define I2C_EV_TX_CMPLT  	 	0
#define I2C_EV_RX_CMPLT  	 	1
#define I2C_EV_STOP       		2
#define I2C_ERROR_BERR 	 		3
#define I2C_ERROR_ARLO  		4
#define I2C_ERROR_AF    		5
#define I2C_ERROR_OVR   		6
#define I2C_ERROR_TIMEOUT 		7
#define I2C_EV_DATA_REQ         8
#define I2C_EV_DATA_RCV         9


//APIs
// Init and DeInit
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_TypeDef *pI2Cx);

//ClockSetup
void I2C_PCLK_CTRL(I2C_TypeDef *pI2Cx, uint8_t EnorDi);

// Data TX and RX
uint8_t I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
uint8_t I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);


//IRQ Handling APIs
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);


// Other Peipheral control APIs
void I2C_PeripheralControl(I2C_TypeDef *pI2cx, uint8_t EnorDi);
uint8_t I2C_GetFlagStatus(I2C_TypeDef *pI2Cx, uint32_t FlagName);
void I2C_ManageAcking(I2C_TypeDef *pI2Cx, uint8_t EnorDi);
void I2C_GenerateStopCondition(I2C_TypeDef *pI2Cx);
uint32_t RCC_GetPCLK1Value(void);

uint8_t I2C_GetFlagStatus(I2C_TypeDef *pI2Cx, uint32_t FlagName);

//Flags
#define I2C_FLAG_SB 		(1 << I2C_SR1_SB_Pos)
#define I2C_FLAG_TXE 		(1 << I2C_SR1_TXE_Pos)
#define I2C_FLAG_RXNE 		(1 << I2C_SR1_RXNE_Pos)
#define I2C_FLAG_ADDR 		(1 << I2C_SR1_ADDR_Pos)
#define I2C_FLAG_BTF 		(1 << I2C_SR1_BTF_Pos)
#define I2C_FLAG_STOPF 		(1 << I2C_SR1_STOPF_Pos)
#define I2C_FLAG_BERR 		(1 << I2C_SR1_BERR_Pos)
#define I2C_FLAG_ARLO 		(1 << I2C_SR1_ARLO_Pos)
#define I2C_FLAG_AF 		(1 << I2C_SR1_AF_Pos)
#define I2C_FLAG_OVR 		(1 << I2C_SR1_OVR_Pos)
#define I2C_FLAG_TIMEOUT 	(1 << I2C_SR1_TIMEOUT_Pos)



//Application callback
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);

#endif /* B3BA84CC_5247_4A36_93E3_D2E2F073F616 */
