#include <stdint.h>
#include "../drivers/delay.h"
#include "../drivers/rcc.h"
#include "../drivers/tim.h"
#include "../drivers/gpio.h"
#include "../drivers/dma.h"
#include "../application/LogicAnalyser_Wrapper.h"
#include "../application/LogicAnalyser_API.h"

typedef enum InputMapping {
  START_ACQUISITION = 0b0001,
  STOP_ACQUISITION = 0b0010,
  INVALID = 0b1111
} InputMapping;

void initialize_getting_signals(LogicAnalyser_API *api);


int main(void) {

  __asm__("CPSIE i");
  //Turn on interrupt for DMA_Stream5
  *((volatile uint32_t*) 0xE000E108) |= (1U << 4);

  //I have these here and that's why I'm importing all the driver.h files
  //however this would defeat the purpose of seperating hardware dependent from independant
  init_rcc_pll();
  turn_on_gpioa();
  turn_on_tim1();
  turn_on_dma2_clock();
  
  //This basically initialized the GPIOA to act as input for jump cables attached to it that bring it input voltage
  initialize_gpio(GPIOA_Input_PullUp);

  LogicAnalyser_API dma_logic_analyzer_instance;
  initialize_getting_signals(&dma_logic_analyzer_instance);

  uint8_t userInput;
  uint8_t dma_acquisition_started = 0;
  uint8_t dma_acquisition_stopped = 1;

  volatile struct dma_bookkeeper* valid_sample;

  while (1) {

    if (userInput == START_ACQUISITION && dma_acquisition_stopped) {
      dma_acquisition_started = get_signals(&dma_logic_analyzer_instance);
      if (dma_acquisition_started) {
        dma_acquisition_stopped = 0;
      }
    }

    if (userInput == STOP_ACQUISITION && dma_acquisition_started) {
      dma_acquisition_stopped = stop_signals(&dma_logic_analyzer_instance);
      if (dma_acquisition_stopped) {
        dma_acquisition_started = 0;
        valid_sample = valid_samples(&dma_logic_analyzer_instance);
      }
    }
    
  }
}