#include <stdint.h>

#include "../drivers/tim.c"
#include "../drivers/dma.c"

typedef enum {
  IDLE,
  STARTING_DMA,
  ACQUIRING,
  STOPPING,
  CAPTURED,
  ERROR
} CAPTURE_STATES;

static CAPTURE_STATES current_capture_state = IDLE;

void start_acquisition(void) {
  if (current_capture_state == ACQUIRING) {
    return;
  }

  if (current_capture_state == IDLE) {
    turn_on_stream5(DMASTREAM5, DMA2);
    current_capture_state = STARTING_DMA;
  }

  if (!(is_dma_enabled(DMASTREAM5))) {
    return;
  }

  start_tim1();
  current_capture_state = ACQUIRING;

}

void stop_acquisition(void) {
  
  if (current_capture_state == IDLE) {
    return;
  }

  if (current_capture_state == ACQUIRING) {
    stop_tim1();
    turn_off_stream5(DMASTREAM5);
    current_capture_state = STOPPING;
  }

  if (!(is_dma_enabled(DMASTREAM5))) {
    current_capture_state = IDLE;
  }

}