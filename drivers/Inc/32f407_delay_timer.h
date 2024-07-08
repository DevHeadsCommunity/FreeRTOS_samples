#ifndef B8C01866_76FF_4A24_BFD0_8D0EC68CE6CE
#define B8C01866_76FF_4A24_BFD0_8D0EC68CE6CE

#include "../../CMSIS/Inc/stm32f407xx.h"
#include <stdint.h>

/*Setup some timers*/
#define MAX_TIMERS 10



typedef struct {
    volatile uint32_t delayCounter;
}DelayTimer_t;


extern uint32_t numTimers;
extern DelayTimer_t *timers[MAX_TIMERS];




void Timer_Init(void);
void Timer_Start(DelayTimer_t *timer, uint32_t milliseconds);
uint8_t Timer_IsElapsed(DelayTimer_t *timer);
void Timer_Update(void);

#endif /* B8C01866_76FF_4A24_BFD0_8D0EC68CE6CE */
