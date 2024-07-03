#include "../../CMSIS/Inc/main.h"
#include <stdint.h>



void Lis3_Init();

void Lis3WriteRead(uint8_t address, uint8_t * read_to_address); 

uint8_t Lis3ReadId();

uint8_t Lis3ReadAxis();