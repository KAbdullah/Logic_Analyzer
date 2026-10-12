#include <stdint.h>
#include "../drivers/gpio.c"

#ifndef tim_lib
#define tim_lib

typedef struct tim_struct {
  uint32_t CR1;
  uint32_t CR2;
  uint32_t SMCR;
  uint32_t DIER;
  uint32_t SR;
  uint32_t EGR;
  uint32_t CCMR1;
  uint32_t CCMR2;
  uint32_t CCER;
  uint32_t CNT;
  uint32_t PSC;

  uint32_t ARR;
  uint32_t RCR;
  uint32_t CCR1;
  uint32_t CCR2;
  uint32_t CCR3;
  uint32_t CCR4;
  uint32_t BDTR;
  uint32_t DCR;
  uint32_t DMAR;

} TIM_STRUCT;

#define TIM1 ((volatile TIM_STRUCT*) 0x40010000)


void send_dma_request() {

  //Prescaler Register PSC
  TIM1->PSC = 14;

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

  //Start the timer
  TIM1->CR1 |= (1U << 0);

}

#endif