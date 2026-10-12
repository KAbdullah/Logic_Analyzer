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


void initialize_gpio(GPIO_Container gpio) {

  for (int pin = 0; pin < 13; pin++) {
    (gpio.address)->MODER &= ~((3 << (pin * 2)));
    (gpio.address)->MODER |= ((gpio.moder << (pin * 2)));

    //Set the voltage driving force
    (gpio.address)->PUPDR &= ~(3 << (pin * 2));
    (gpio.address)->PUPDR |= (gpio.pupdr << (pin * 2));
  }
}

GPIO_Container GPIOA_Input_PullUp = {
  .address = GPIOA,
  .moder = 0,
  //No floating, bring line to high, both I2C and UART bring lines to high
  .pupdr = 1
};


#endif