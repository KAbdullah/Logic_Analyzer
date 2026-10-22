#include <stdint.h>

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

void configuration_of_tim1();

void start_tim1(void);

void stop_tim1(void);

#endif