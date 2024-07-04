#ifndef C789DC2E_6DCB_4711_A0A3_ECCDFF51983F
#define C789DC2E_6DCB_4711_A0A3_ECCDFF51983F
#include "../../CMSIS/Inc/stm32f407xx.h"
#include <stdint.h>

// LIS3DSH register addresses
#define LIS3DSH_CTRL_REG4  0x20
#define LIS3DSH_CTRL_REG5  0x24
#define LIS3DSH_CTRL_REG6  0x25
#define LIS3DSH_OUT_X_L    0x28
#define LIS3DSH_OUT_X_H    0x29
#define LIS3DSH_OUT_Y_L    0x2A
#define LIS3DSH_OUT_Y_H    0x2B
#define LIS3DSH_OUT_Z_L    0x2C
#define LIS3DSH_OUT_Z_H    0x2D
#define LIS3DSH_OUT_T      0x0C

#define SENSITIVITY_2G  0.06  // 0.06 mg/LSB * 1000
#define SENSITIVITY_4G  0.120 // 0.12 mg/LSB * 1000
#define SENSITIVITY_6G  0.180 // 0.18 mg/LSB * 1000
#define SENSITIVITY_8G  0.240 // 0.24 mg/LSB * 1000
#define SENSITIVITY_16G 0.730 // 0.73 mg/LSB * 1000

typedef struct
{
	uint8_t Lis3_DR;
    uint8_t Lis3_BDU;
    uint8_t Lis3_Axes;
    float Lis3_Sensitivity;
}Lis3_Config_t;

void Lis3_Init(Lis3_Config_t Config);

void Lis3WriteRead(uint8_t address, uint8_t * read_to_address); 
void Lis3Write(uint8_t address, uint8_t data); 

uint8_t Lis3ReadId();

float Lis3ReadAxis(char axis);

int8_t Lis3ReadTemp();


#endif /* C789DC2E_6DCB_4711_A0A3_ECCDFF51983F */
