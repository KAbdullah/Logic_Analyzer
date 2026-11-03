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
#define GPIOB ((volatile GPIO_STRUCT *) 0x40020400)
#define GPIOC ((volatile GPIO_STRUCT *) 0x40020800)
#define GPIOD ((volatile GPIO_STRUCT *) 0x40020C00)
#define GPIOE ((volatile GPIO_STRUCT *) 0x40021000)
#define GPIOF ((volatile GPIO_STRUCT *) 0x40021400)
#define GPIOG ((volatile GPIO_STRUCT *) 0x40021800)
#define GPIOH ((volatile GPIO_STRUCT *) 0x40021C00)

typedef struct gpio_container {
  volatile GPIO_STRUCT *address;
  uint32_t moder;
  uint32_t pupdr;

  //For the advanced part
  uint8_t pin1;
  uint8_t pin2;
  uint32_t otyper;
  uint32_t ospeedr;
  uint32_t afrl;
} GPIO_Container;


void initialize_gpio(GPIO_Container gpio);
void initialize_gpio_advanced(GPIO_Container gpio);

extern GPIO_Container GPIOC_Input_PullUp;
extern GPIO_Container UsingUSART2;

#endif