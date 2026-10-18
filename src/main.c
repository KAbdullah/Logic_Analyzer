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
  
  initialize_gpio(GPIOA_Input_PullUp);
  send_dma_request();

  dma_stream_configuration(DMASTREAM5);
  turn_on_stream1(DMASTREAM5);

  uint8_t *head = (uint8_t *) 0x2000F800;
  uint32_t currentIndex = 0;
  uint32_t currentIndexLog = 0;
  uint8_t data[15000];

  while (1) {

    if (currentIndex >= 65528) {
      head = (uint8_t *) 0x2000F800;
      currentIndex = 0;
    }

    if (currentIndexLog >= 15000) {
      currentIndexLog = 0;
    }

    
    head++;
    data[currentIndexLog] = *head;
    currentIndex++;
    currentIndexLog++;
    

  }
}