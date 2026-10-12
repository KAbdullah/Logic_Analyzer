#include <stdint.h>
#include "../drivers/rcc.c"
#include "../drivers/tim.c"
#include "../drivers/gpio.c"

int main(void) {

  init_rcc_pll();
  turn_on_gpioa();
  turn_on_tim1();
  
  initialize_gpio(GPIOA_Input_PullUp);
  send_dma_request();

  while (1) {
    
  }
}