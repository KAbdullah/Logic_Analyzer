#include <stdint.h>
#include "../drivers/delay.c"
#include "../drivers/rcc.c"
#include "../drivers/tim.c"
#include "../drivers/gpio.c"
#include "../drivers/dma.c"
#include "../manager/startandstop.c"

int main(void) {

  init_rcc_pll();
  turn_on_gpioa();
  turn_on_tim1();
  turn_on_dma2_clock();
  
  //This basically initialized the GPIOA to act as input for jump cables attached to it that bring it input voltage
  initialize_gpio(GPIOA_Input_PullUp);

  while (1) {
    start_acquisition();

    stop_acquisition();

  }
}