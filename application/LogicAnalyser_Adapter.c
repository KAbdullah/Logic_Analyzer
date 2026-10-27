#include <stdint.h>

#include "../drivers/tim.h"
#include "../drivers/dma.h"
#include "LogicAnalyser_API.h"

volatile struct dma_bookkeeper BOOK = {
  .buffer_wrapping = 0,
  .first_data_pointer = (uint8_t*) 0x2000F800,
  .completed_count = 0
};

struct LogicAnalyserConfig {
  volatile DMA_STREAM* dma_stream;
  volatile DMA_REGISTERS* general_dma;
  volatile struct dma_bookkeeper* dma_bookkeeping;
};

static struct LogicAnalyserConfig dma_configuration_2 = {
  .dma_stream = DMASTREAM5,
  .general_dma = DMA2,
  .dma_bookkeeping = &BOOK
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

static int dma_start_acquisition(struct LogicAnalyserConfig *handle) {

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

static int dma_stop_acquisition(struct LogicAnalyserConfig *handle) {
  
  if (current_capture_state == IDLE) {
    return 0;
  }

  if (current_capture_state == ACQUIRING) {
    stop_tim1();
    turn_off_stream5(handle->dma_stream);
    current_capture_state = STOPPING;
  }

  if (!(is_dma_enabled(handle->dma_stream))) {
    current_capture_state = CAPTURED;
  } else {
    return 0;
  }

  return 1;

}

static volatile struct dma_bookkeeper* dma_bookkeeping(struct LogicAnalyserConfig *handle) {
  if (current_capture_state == CAPTURED) {
    count_samples(handle->dma_stream, handle->dma_bookkeeping);
    current_capture_state = IDLE;
  }

  return handle->dma_bookkeeping;
}

void initialize_getting_signals(LogicAnalyser_API *api) {
  api->context = &dma_configuration_2;
  api->start_acquisition = dma_start_acquisition;
  api->stop_acquisition = dma_stop_acquisition;
  api->bookeeping = dma_bookkeeping;
}