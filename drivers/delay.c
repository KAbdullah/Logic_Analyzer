#include <stdint.h>
#include "delay.h"


void Systick_init() {
  SysTick->CTRL |= (1 << 2);
  SysTick->CTRL &= ~(1 << 1);
  SysTick->CTRL |= (1 << 0);
} 

// Using a 16MHz clock, 16,000,000 clocks happen every second
// 180,000,000 / 1000 = 180,000 clock cycles happen every millisecond
void delay(uint32_t milliseconds) {
  SysTick->LOAD = (milliseconds * 180000) - 1;
  SysTick->VAL = 0;

  Systick_init();

  while (!(SysTick->CTRL & (1 << 16))) {
    //Wait until the flag becomes 1
  }

}