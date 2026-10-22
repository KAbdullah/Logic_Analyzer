#include <stdint.h>

#ifndef gpio_lib
#define gpio_lib

typedef struct gpio {
  uint32_t MODER;
  uint32_t OTYPER;
  uint32_t OSPEEDER;
  uint32_t PUPDR;
  uint32_t IDR;
  uint32_t ODR;
  uint32_t BSRR;
  uint32_t LCKR;
  uint32_t AFRL;
  uint32_t AFRH;  
} GPIO_STRUCT;

#define GPIOA ((volatile GPIO_STRUCT*) 0x40020000)

typedef struct gpio_container {
  volatile GPIO_STRUCT *address;
  uint32_t moder;
  uint32_t pupdr;
} GPIO_Container;


void initialize_gpio(GPIO_Container gpio);

extern GPIO_Container GPIOA_Input_PullUp;

#endif