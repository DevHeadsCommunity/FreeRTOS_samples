#include "../Inc/32f407_delay_timer.h"
#include <stdio.h>

uint32_t numTimers = 0;
DelayTimer_t *timers[MAX_TIMERS];


/**
 * @brief Timer_Init
 * Initializes the timer functionality for our system.
 * At the moment this assumes we are running at a clock speed of 
 * 16MHz and will make systick at the rate of once per Ms.
 */
void Timer_Init(void) {
    SysTick_Config(SystemCoreClock / 1000);
}

/**
 * @brief Timer_Start
 * Function to start a specific DelayTimer, recall each DelayTimer 
 * has a delay countetr and by starting it we assign it a number of milliseconds 
 * then let the systick interrupt decrement those ms until they are 0
 * 
 * @param timer 
 * @param milliseconds 
 */
void Timer_Start(DelayTimer_t *timer, uint32_t milliseconds) {
    timer->delayCounter = milliseconds;
}

/**
 * @brief Timer_IsElapsed
 * Api Function to check when a timer is elapsed.
 * Since DelayTimers is global this accepts an index of a timer 
 * and uses that index to pick a correct timer to check.
 * 
 * @param timerIndex 
 * @return uint8_t 
 */
uint8_t Timer_IsElapsed(DelayTimer_t *timer) {
    return(timer->delayCounter == 0);
}

/**
 * @brief SysTick_Hanlder
 * ISR for systick interrupt.
 
 Now Handled in FreeRtos hook
void SysTick_Handler(void) {
    Timer_Update();
}
*/


/**
 * @brief Timer_Update
 * Internal function consumed from the Systick Handler, So basically this 
 * is run at the rate of once every systick cyle.
 * Goes through the list of current active timers and decrements their ms 
 * values
 * 
 */
void Timer_Update(void) {
    // start a critical section of code
    __disable_irq();
    for (uint32_t i = 0; i < numTimers; i++) {
        if (timers[i]->delayCounter > 0) {
            timers[i]->delayCounter--;
        }
    }
    // complete the critical section of code 
    __enable_irq();
}
