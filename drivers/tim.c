#include <stdint.h>
#include "tim.h"
#include "../drivers/gpio.h"

//Timer will be generate a DMA interrupt every 10MHz, very safe for DMA to accomplish its writes on the RAM

void configuration_of_tim1() {

  //Prescaler Register PSC
  TIM1->PSC = 8;

  //Auto-reload Register ARR
  TIM1->ARR = 1;

  //Repetition counter Register = 1 RCR
  TIM1->RCR = 0;

  //Count down 
  TIM1->CR1 |= (1U << 4);

  //Enable DMA request
  TIM1->DIER |= (1U << 8);

  //Reinitialized counter register and update registers (brings value from shadow registers)
  TIM1->EGR |= (1U << 0);

}

void start_tim1(void) {
  configuration_of_tim1();
  //Start the timer
  TIM1->CR1 |= (1U << 0);
}

void stop_tim1(void) {
  //Start the timer
  TIM1->CR1 &= ~(1U << 0);
}