#include <stdint.h>
#include "gpio.h"


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
