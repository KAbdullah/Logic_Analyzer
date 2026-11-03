#include <stdint.h>
#include "uart.h"

void uart_general_init(volatile UART_Struct *uart_address) {
  //Enable UART UE bit 
  uart_address->CR1 |= (1U << 13);

  //Program M bit to define word length
  uart_address->CR1 &= ~(1U << 12);

  //Program the number of stop bits
  uart_address->CR2 &= ~(3U << 12);

  //Select DMA enable if multibuffer communication is to take place
  uart_address->CR3 &= ~(1U << 6);
  uart_address->CR3 &= ~(1U << 7);

  //Select Desired Buad rate of 921.6 KBps
  uart_address->BRR = (12U << 4) | (2U << 0); 

  //Set the TE bit in UART CR1 to send and idle frame as first transmission
  uart_address->CR1 |= (1 << 3);

}

//2KB buffer to brace for state change in cluster
uint8_t UART_BUFFER[2048];

uint16_t head = 0;
uint16_t tail = 0;
uint8_t *current;
uint8_t buffer_full;
uint32_t dropped_samples; 
uint8_t buffer_overflow;

//For the current state right, we're only extracting the first two bits 
//For current state length we check if it's bigger than 2^22 - 1,
// 1) If not then we send 3 bytes, 1st byte with first two bits as state, then the next 6 bits contain the current length from 2^3 to 2^8
// then the second bit values will be counted as 2^9 to 2^16 then the third bit is from 2^17 to 2^24
uint8_t send_uart_data(uint8_t current_state, uint32_t current_state_length) {
  uint16_t h = head;
  //If length is 3, then 3 / 2^22 is 0; but 3 + 2^22 - 1 will give one, because maximum it can ever be 2^22 2^22 - 1 / 2^22 = 1
  uint32_t chunks = (current_state_length + 4194302U) / 4194303; 

  //Need to have a way to check that I can actually send data to the buffer
  //Free space = (tail - head + 2047) % 2048
  if (((tail - head + 2047) % 2048) < (3 * chunks)) { 
    buffer_overflow = 1;
    dropped_samples++;
    return 0;
  }  
  
  //Add data to the buffer
  while (current_state_length > 0) {
    uint32_t chunk = (current_state_length > 4194303U) ? 4194303U : current_state_length;
    uint8_t first_byte = (current_state & (3U << 0)) | ((chunk << 2) &  0b11111100);
    uint8_t second_byte = (chunk >> 6) & 0b11111111;
    uint8_t third_byte = (chunk >> 14) & 0b11111111;

    
    UART_BUFFER[h] = first_byte;
    h = (h + 1) % 2048;
    UART_BUFFER[h] = second_byte;
    h = (h + 1) % 2048;
    UART_BUFFER[h] = third_byte;
    h = (h + 1) % 2048;

    current_state_length -= chunk;
  };

  head = h;
  //Enable Interrupt
  USART2->CR1 |= (1U << 7);
  return 1;
}

void USART1_IRQHandler(void) {


  //Write the data to send in the UART DR (which clears the TXE bit). - Will probably be moved to interrupt

  //After writing the last data, wait until TC=1. this indicates the transmission of the last frame is complete. - Will probably be moved to interrupt

}
