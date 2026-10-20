#include <stdint.h>
#include "../drivers/rcc.c"
#include "../drivers/tim.c"
#include "../drivers/gpio.c"
#include "../drivers/dma.c"

int main(void) {

  init_rcc_pll();
  turn_on_gpioa();
  turn_on_tim1();
  turn_on_dma2_clock();
  
  //This basically initialized the GPIOA to act as input for jump cables attached to it that bring it input voltage
  initialize_gpio(GPIOA_Input_PullUp);
  send_dma_request();

  dma_stream_configuration(DMASTREAM5);
  turn_on_stream1(DMASTREAM5);

  //Filling our little buffer here
  uint8_t *head = (uint8_t *) 0x2000F800;
  uint32_t currentIndex = 0;

  uint32_t starting_procedures = 0;
  uint32_t stoping_procedures = 0;

  while (1) {

    if (currentIndex >= 65528) {
      currentIndex = 0;
    }

    if (currentIndex < 1) {
      currentIndex++;
      continue;
    }

    uint32_t previous_state = 0b00000011 & head[currentIndex - 1];
    uint32_t current_state = 0b00000011 & head[currentIndex];

    if ((previous_state == 3U) & (current_state == 1U)) {
      starting_procedures++;
    }

    if ((previous_state == 1U) & (current_state == 3U)) {
      stoping_procedures++;
    }
    
    currentIndex++;

  }
}