#ifndef B8C01866_76FF_4A24_BFD0_8D0EC68CE6CE
#define B8C01866_76FF_4A24_BFD0_8D0EC68CE6CE

#include "../../CMSIS/Inc/stm32f407xx.h"

typedef struct {
    uint32_t delayCounter;
}DelayTimer_t;


void Timer_Init(void);
void Timer_Start(DelayTimer_t *timer, uint32_t milliseconds);
uint8_t Timer_IsElapsed(DelayTimer_t *timer);
void Timer_Update(void);

#endif /* B8C01866_76FF_4A24_BFD0_8D0EC68CE6CE */
