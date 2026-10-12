#include <stdint.h>

typedef struct  {

  struct {
    uint32_t STREAM_ADDRESS;
  };

  uint32_t LISR;
  uint32_t HISR;
  uint32_t LIFCR;
  uint32_t HIFCR;
  uint32_t SCR;
  uint32_t SNDTR;
  uint32_t SPAR;
  uint32_t SM0AR;
  uint32_t SM1AR;
  uint32_t SFCR;
} DMA_REGISTERS;



void dma_stream_configuration() {
  //Reset the EN bit
  
}