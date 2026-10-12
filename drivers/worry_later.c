#include <stdint.h>

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
  uint8_t pin1;
  uint8_t pin2;
  uint32_t moder;
  uint32_t otyper;
  uint32_t ospeeder;
  uint32_t pupdr;
  uint32_t afrl;
} GPIO_Container;


void initialize_gpio(GPIO_Container gpio) {
  (gpio.address)->MODER &= ~((3 << (gpio.pin1 * 2)) | (3 << (gpio.pin2 * 2)));
  (gpio.address)->MODER |= ((gpio.moder << (gpio.pin1 * 2)) | (gpio.moder << (gpio.pin2 *2)));

  //Set to Open Drain
  (gpio.address)->OTYPER &= ~((1 << gpio.pin1) | (1 << gpio.pin2));
  (gpio.address)->OTYPER |= (gpio.otyper << gpio.pin1);
  //UART4 is on GPIOA, and RX is input only so it doesn't get PMOS and NMOS (push-pull) functionality
  if (gpio.address != GPIOA) {
    (gpio.address)->OTYPER |= (gpio.otyper << gpio.pin2);
  }

  //Set speed of tranmission of the GPIO
  (gpio.address)->OSPEEDER &= ~((0b11 << (gpio.pin1 * 2)) | (0b11 << (gpio.pin2 *2)));
  (gpio.address)->OSPEEDER |= ((gpio.ospeeder << (gpio.pin1 * 2)) | (gpio.ospeeder << (gpio.pin2 *2)));

  //Set the voltage driving force
  (gpio.address)->PUPDR &= ~((3 << (gpio.pin1 * 2)) | (3 << (gpio.pin2 *2)));
  (gpio.address)->PUPDR |= (((gpio.address != GPIOA) ? (gpio.pupdr << (gpio.pin1 * 2)) : 0) | (gpio.pupdr << (gpio.pin2 *2)));

  //Which pins to set as alternative functions
  (gpio.address)->AFRL &= ~((15 << (gpio.pin1 * 4)) | (15 << (gpio.pin2 * 4)));
  (gpio.address)->AFRL |= ((gpio.afrl << (gpio.pin1 * 4)) | (gpio.afrl << (gpio.pin2 * 4)));

  //Sequence to lock the configuration
  (gpio.address)->LCKR = (1 << 16) | (1 << gpio.pin1) | (1 << gpio.pin2);
  (gpio.address)->LCKR = (0 << 16) | (1 << gpio.pin1) | (1 << gpio.pin2);
  (gpio.address)->LCKR = (1 << 16) | (1 << gpio.pin1) | (1 << gpio.pin2);
  (void)(gpio.address)->LCKR;
}