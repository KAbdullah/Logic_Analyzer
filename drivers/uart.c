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
  uart_address->BRR = (3U << 4) | (1U << 0); 

  //Set the TE bit in UART CR1 to send and idle frame as first transmission
  uart_address->CR1 |= (1 << 3);

}

//2KB buffer to brace for state change in cluster
uint8_t UART_BUFFER[2048];

volatile uint16_t head = 0;
volatile uint16_t tail = 0;
uint32_t dropped_samples = 0; 
uint8_t buffer_overflow;

//For the current state right, we're only extracting the first two bits 
//For current state length we check if it's bigger than 2^22 - 1,
// 1) If not then we send 3 bytes, 1st byte with first two bits as state, then the next 6 bits contain the current length from 2^3 to 2^8
// then the second bit values will be counted as 2^9 to 2^16 then the third bit is from 2^17 to 2^24
uint8_t send_uart_data(uint8_t current_state, uint32_t current_state_length) {
  volatile uint16_t h = head;
  //If length is 3, then 3 / 2^19 is 0; but 3 + 2^19 - 1 will give one, because maximum it can ever be 2^19 + 2^19 - 1 / 2^19 = 1
  uint32_t chunks = (current_state_length + 524286U) / 524287U; 

  //Need to have a way to check that I can actually send data to the buffer
  //Free space = (tail - head + 2047) % 2048
  if (((tail - head + 2047U) % 2048U) < (3 * chunks)) { 
    buffer_overflow = 1;
    dropped_samples += (3 * chunks);
    return 0;
  }  
  
  //Add data to the buffer
  while (current_state_length > 0) {
    uint32_t chunk = (current_state_length > 524287U) ? 524287U : current_state_length;
    //First byte is broken like this: first 2 bits is state, then next 5 bits is count, then 8th bit is head flag
    uint8_t first_byte = (current_state & (3U << 0)) | ((chunk << 2) &  0b01111100) | ((1U << 7));
    uint8_t second_byte = (chunk >> 5) & 0b01111111;
    uint8_t third_byte = (chunk >> 12) & 0b01111111;

    
    UART_BUFFER[h] = first_byte;
    h = (h + 1) % 2048U;
    UART_BUFFER[h] = second_byte;
    h = (h + 1) % 2048U;
    UART_BUFFER[h] = third_byte;
    h = (h + 1) % 2048U;

    current_state_length -= chunk;
  };

  head = h;
  //Enable Interrupt
  USART2->CR1 |= (1U << 7); //Turn on interrupt for TXE bit
  return 1;
}

void USART2_IRQHandler(void) {
  volatile uint16_t t = tail;

  //tail == head ? buffer empty; tail - 1 == head ? buffer full;
  if (((head - tail + 2048U) % 2048U) == 0) {
    //Turn off interrupt
    USART2->CR1 &= ~(1U << 7); //Turn off interrupt for TXE bit
    return;
  }

  //Write the data to send in the UART DR (which clears the TXE bit). - Will probably be moved to interrupt
  USART2->DR = UART_BUFFER[tail];
  t = (t + 1) % 2048U;

  tail = t;
}
