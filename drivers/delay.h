#include <stdint.h>

#ifndef STM32Sleep
#define STM32Sleep

typedef struct {
  uint32_t CTRL;
  uint32_t LOAD;
  uint32_t VAL;
  uint32_t CALIB;
} STK_Struct;

#define SysTick ((volatile STK_Struct *) 0xE000E010)

void Systick_init();

void delay(uint32_t milliseconds);

#endif