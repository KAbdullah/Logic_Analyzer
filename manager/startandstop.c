#include <stdint.h>

#include "../drivers/tim.c"
#include "../drivers/dma.c"

typedef enum {
  IDLE,
  ACQURING,
  COMPLETED
} CAPTURE_STATES;

static CAPTURE_STATES current_capture_state = IDLE;

void start_acquisition(void) {
  if (current_capture_state == ACQURING) {
    return;
  }

  current_capture_state = ACQURING;
  start_tim1();
  turn_on_stream1(DMASTREAM5);

}

void stop_acquisition(void) {

}