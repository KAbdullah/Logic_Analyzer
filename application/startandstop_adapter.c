#include <stdint.h>

#include "../drivers/tim.h"
#include "../drivers/dma.h"
#include "startandstop_api.h"

struct StartAndStopConfig {
  volatile DMA_STREAM* dma_stream;
  volatile DMA_REGISTERS* general_dma;
};

static struct StartAndStopConfig dma_configuration_2 = {
  .dma_stream = DMASTREAM5,
  .general_dma = DMA2
};

typedef enum {
  IDLE,
  STARTING_DMA,
  ACQUIRING,
  STOPPING,
  CAPTURED,
  ERROR
} CAPTURE_STATES;

static CAPTURE_STATES current_capture_state = IDLE;

static int dma_start_acquisition(void *context) {

  struct StartAndStopConfig* handle = context;

  if (current_capture_state == ACQUIRING) {
    return 0;
  }

  if (current_capture_state == IDLE) {
    turn_on_stream5(handle->dma_stream, handle->general_dma);
    current_capture_state = STARTING_DMA;
  }

  if (!(is_dma_enabled(handle->dma_stream))) {
    return 0;
  }

  start_tim1();
  current_capture_state = ACQUIRING;

  return 1;

}

static int dma_stop_acquisition(void *context) {

  struct StartAndStopConfig* handle = context;
  
  if (current_capture_state == IDLE) {
    return 0;
  }

  if (current_capture_state == ACQUIRING) {
    stop_tim1();
    turn_off_stream5(handle->dma_stream);
    current_capture_state = STOPPING;
  }

  if (!(is_dma_enabled(handle->dma_stream))) {
    current_capture_state = IDLE;
  } else {
    return 0;
  }

  return 1;

}

void initialize_getting_signals(StartAndStop_API *api) {
  api->context = &dma_configuration_2;
  api->start_acquisition = dma_start_acquisition;
  api->stop_acquisition = dma_stop_acquisition;
}