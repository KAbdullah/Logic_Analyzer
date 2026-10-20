#include <stdint.h>
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

  //This is the timer sending DMA requests at predefined intervals
  configuration_of_tim1();

  //This is the DMA configuration 
  dma_stream_configuration(DMASTREAM5);
  __asm__("nop");

  start_acquisition();

  while (1) {

  }
}