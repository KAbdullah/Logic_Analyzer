#ifndef DEFINE_DMA
#define DEFINE_DMA

typedef struct  {
  uint32_t LISR;
  uint32_t HISR;
  uint32_t LIFCR;
  uint32_t HIFCR;
} DMA_REGISTERS;

typedef struct {
  uint32_t SCR;
  uint32_t SNDTR;
  uint32_t SPAR;
  uint32_t SM0AR;
  uint32_t SM1AR;
  uint32_t SFCR;
} DMA_STREAM;

struct dma_bookkeeper {
  uint8_t buffer_wrapping;
  uint8_t* dma_data_pointer;
  uint16_t completed_count; 
};

#define DMA2ADDRESS 0x40026400

#define DMA2 ((volatile DMA_REGISTERS *) 0x40026400)
#define DMASTREAM5 ((volatile DMA_STREAM *) (DMA2ADDRESS + 0x010 + (0x18 * 5)))

extern int complete_transfer_occured;

void dma_stream_configuration(volatile DMA_STREAM* dma_stream, volatile DMA_REGISTERS* general_dma);

void turn_on_stream5(volatile DMA_STREAM* dma_stream, volatile DMA_REGISTERS* general_dma);

void turn_off_stream5(volatile DMA_STREAM* dma_stream);

int is_dma_enabled(volatile DMA_STREAM* dma_stream);

void count_samples(volatile DMA_STREAM* dma_stream, volatile struct dma_bookkeeper* book_keeping);

#endif