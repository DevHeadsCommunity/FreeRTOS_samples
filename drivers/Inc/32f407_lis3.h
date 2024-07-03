#ifndef C789DC2E_6DCB_4711_A0A3_ECCDFF51983F
#define C789DC2E_6DCB_4711_A0A3_ECCDFF51983F
#include "../../CMSIS/Inc/main.h"
#include <stdint.h>

typedef struct
{
	uint8_t Lis3_DR;
    uint8_t Lis3_BDU;
    uint8_t Lis3_Axes;
}Lis3_Config_t;

void Lis3_Init(Lis3_Config_t Config);

void Lis3WriteRead(uint8_t address, uint8_t * read_to_address); 
void Lis3Write(uint8_t address, uint8_t data); 

uint8_t Lis3ReadId();

uint8_t Lis3ReadAxis();


#endif /* C789DC2E_6DCB_4711_A0A3_ECCDFF51983F */
