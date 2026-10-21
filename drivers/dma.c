#include <stdint.h>

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

#define DMA2ADDRESS 0x40026400

#define DMA2 ((volatile DMA_REGISTERS *) 0x40026400)
#define DMASTREAM5 ((volatile DMA_STREAM *) (DMA2ADDRESS + 0x010 + (0x18 * 5)))

//This needs to be on the outside so that we can only call the above function if EN bit is 0
//If the bit is on if we do (1 << 5) for example, the value would be 32, meaning on, if it's off then it would be 0, therefore no comparator is still necessary
// if ((DMA2->SCR & (1 << 0))) {
//
// }

void dma_stream_configuration(volatile DMA_STREAM* dma_stream, volatile DMA_REGISTERS* general_dma) {

  if ((dma_stream->SCR & (1 << 0)) != 0) {
    return;
  }

  //Clear the DMA complete and half transfer flags
  general_dma->HIFCR |= (1U << 11);
  general_dma->HIFCR |= (1U << 10);

  //The peripheral address is the GPIOA IDR register
  dma_stream->SPAR = 0x40020010;

  //The data is written to this peice of memory in SRAM1, set Length in Linker to 62KB, so this is bascially SRAM1 base address + 62KB aka 63,488 bytes (0xF800) 
  dma_stream->SM0AR = 0x2000F800;

  //Decrement the NDTR, even though we'll do single burst, I want to keep the amount transferred a multiple of our 1 byte data
  dma_stream->SNDTR = 65528;

  //Select DMA channel here, Channel 6 for TIM1 in DMA2
  dma_stream->SCR |= (6U << 25);

  //The DMA is the flow controller
  dma_stream->SCR &= ~(1 << 5);

  //Stream priority in SCR regsiter PL[1:0] bit - High because there isn't any other channels
  dma_stream->SCR |= (2U << 16);

  //For FIFO disable direct mode bit DMDIS in SFCR
  dma_stream->SFCR |= (1U << 2);

  //Fifo treshold FTH[1:0] in SFCR, doesn't matter arbitrary 00 (1/4)
  dma_stream->SFCR |= (0U << 0);

  //Direction can be set in DIR[1:0] in SCR 00 -> Perph to Mem and 01 Mem to Perph
  dma_stream->SCR |= (0U << 6);

  //Data width needs to be programmed PSIZE or MSIZE in SCR regsiter
  dma_stream->SCR |= (0U << 11);
  dma_stream->SCR |= (0U << 13);

  //Disable pointer increment through the PINC and MINC bit in SCR
  dma_stream->SCR &= ~(1U << 9);
  dma_stream->SCR |= (1U << 10);

  //The PINCOS bit being set forces increments of 4 bytes, but turning them off increments them by PSIZE, don't care since the peripheral stays the same
  dma_stream->SCR &= ~(1U << 15);

  //Enable Circular mode using CIRC bit in SCR if so then you must 
  //1) SNDTR = Multiple of (MBurst * (Msize/Psize)) where
  //2) Mburst in SCR, we'll do 4, because I2C and UART are slow enough that 16 would be a waste
  //3) Msize and Psize are both going to be 00, 1 byte
  //4) NDTR is going to be a multiple of 4* 8/8 = 4; So in our case it'll be 65,528.

  dma_stream->SCR |= (1U << 8);

  //Scratch the above, byte transfers are useless because we sample 25 times faster anyways, sampling 4 continuous clock cycles doesn't achieve anything for us
  //MBURST and PBURST in SCR will be 00, single transfer
  dma_stream->SCR &= ~(1U << 21);
  dma_stream->SCR &= ~(1U << 23);  
}

void turn_on_stream5(volatile DMA_STREAM* dma_stream, volatile DMA_REGISTERS* general_dma) {
  dma_stream_configuration(dma_stream, general_dma);

  dma_stream->SCR |= (1U << 0);
}

void turn_off_stream5(volatile DMA_STREAM* dma_stream) {
  dma_stream->SCR &= ~(1U << 0);
}

int is_dma_enabled(volatile DMA_STREAM* dma_stream) {
  if (dma_stream->SCR & (1U << 0)) {
    return 1;
  } else {
    return 0;
  }
}


//Interupt: TCIF -> Stream transfer complete 

#endif