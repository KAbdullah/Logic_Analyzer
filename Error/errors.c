#include "../drivers/tim.h"
#include "../drivers/dma.h"
#include "errors.h"

volatile enum SYSTEM_ERRORS current_error_state = ERROR_FREE;

void handle_transfer_error() {
  current_error_state = DMA_TRANSFER_ERROR;

  stop_tim1();
  turn_off_stream5(DMASTREAM5);

  //No more interrupts for TEIF
  DMASTREAM5->SCR &= ~(1U << 1);

  //Clear TEIF flag
  DMA2->HIFCR = (1U << 9);
  
  return;
}
