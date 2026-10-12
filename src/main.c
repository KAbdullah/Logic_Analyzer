#include <stdint.h>
#include "../drivers/rcc.c"
#include "../drivers/tim.c"
#include "../drivers/gpio.c"

int main(void) {

  init_rcc_pll();
  turn_on_gpioa();
  turn_on_tim1();
  
  // initialize_gpio(GPIOA_Input_PullUp);
  send_dma_request();

  //THIS is for the led blinking
  GPIOA->MODER &= ~(3 << 10);
  GPIOA->MODER |= (1 << 10);

  GPIOA->OTYPER &= ~(1 << 5);

  GPIOA->OSPEEDER &= ~(3 << 10);

  GPIOA->PUPDR &= ~(3 << 10);

  while (1) {

    if ((TIM1->SR & (1U << 0)) != 0U) {
      TIM1->SR &= ~(1U << 0);

      if ((GPIOA->ODR & (1U << 5)) != 0U) {
        GPIOA->BSRR = (1U << 21);
      } else {
        GPIOA->BSRR = (1U << 5);
      }
    }
  }
}