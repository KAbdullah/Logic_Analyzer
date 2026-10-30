#include <stdint.h>
#include "../drivers/delay.h"
#include "../drivers/rcc.h"
#include "../drivers/tim.h"
#include "../drivers/gpio.h"
#include "../drivers/dma.h"
#include "../application/LogicAnalyser_Wrapper.h"
#include "../application/LogicAnalyser_API.h"

typedef enum InputMapping {
  I2C_START_ACQUISITION = 0b0001,
  I2C_STOP_ACQUISITION = 0b0010,
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
  uint8_t previous_state = 0b00000000;
  uint8_t* CPU_pointer = (uint8_t *) 0x2000F800;
  
  uint16_t maximum_run_length = 0;
  uint16_t current_run_length = 0;
  uint16_t number_of_raw_samples = 0;
  uint16_t number_of_grouped_states = 0;

  while (1) {

    if (userInput == I2C_START_ACQUISITION && dma_acquisition_stopped) {
      dma_acquisition_started = get_signals(&dma_logic_analyzer_instance);
      if (dma_acquisition_started) {
        dma_acquisition_stopped = 0;
      }
    }

    //if (dma_acquisition_started)
    if (userInput == I2C_STOP_ACQUISITION && dma_acquisition_started) {
      dma_acquisition_stopped = stop_signals(&dma_logic_analyzer_instance);
      if (dma_acquisition_stopped) {
        dma_acquisition_started = 0;
      }
    }

    //Test taking place

    if (userInput == I2C_STOP_ACQUISITION || userInput == I2C_START_ACQUISITION) {
      
      valid_sample = valid_samples(&dma_logic_analyzer_instance);

      if (CPU_pointer < valid_sample->dma_data_pointer) {
        
        if (previous_state == *CPU_pointer) {
          current_run_length++;
        } else {
          if (maximum_run_length < current_run_length) {
            maximum_run_length = current_run_length;
            current_run_length = 0;
          }
          previous_state = *CPU_pointer;
          number_of_grouped_states++;
        }

        number_of_raw_samples++;

        CPU_pointer++;
      }

      // if (valid_sample->buffer_wrapping) {
      //   for (int i = 0; i < 65528 - valid_sample->completed_count; i++) {
      //     current_pointer += i;

      //     if (previous_state != *current_pointer) {
      //       //Send to UART

      //       //The current pointer data

      //       //Number of times the data was the same previously RLE (Run-Length Encoding)
      //     }
      //   }
      // }
    }
    
  }
}