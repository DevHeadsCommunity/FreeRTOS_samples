#include "../Inc/32f407_delay_timer.h"
#include <stdio.h>

uint32_t numTimers = 0;
DelayTimer_t timers[MAX_TIMERS];


//Initialize Systick timer assumes 16MHz
// Initialize SysTick timer (assuming system clock is set to 16 MHz)
void Timer_Init(void) {
    SysTick_Config(SystemCoreClock / 1000);
}

// Start a non-blocking delay
void Timer_Start(DelayTimer_t *timer, uint32_t milliseconds) {
    timer->delayCounter = milliseconds;
}

// Check if the delay has elapsed
uint8_t Timer_IsElapsed(DelayTimer_t *timer,  uint32_t numTimer) {
	uint32_t timy = timers[numTimer].delayCounter;
    return (timy == 0);
}

// SysTick Handler to update all active timers
void SysTick_Handler(void) {
    Timer_Update();
}

// Update all active timers
void Timer_Update(void) { 
    for (uint32_t i = 0; i < numTimers; i++) {
        if (timers[i].delayCounter > 0) {
            printf("Timer %lu: delayCounter = %lu\n", i, timers[i].delayCounter); // Debug output
            timers[i].delayCounter--;
        }
    }
}
