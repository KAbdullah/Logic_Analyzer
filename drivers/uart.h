#include <stdint.h>

#ifndef UART_DRIVER
#define UART_DRIVER

typedef struct {
  uint32_t SR;
  uint32_t DR;
  uint32_t BRR;
  uint32_t CR1;
  uint32_t CR2;
  uint32_t CR3;
  uint32_t GTPR;
} UART_Struct;

typedef enum {
  IDLE,
  RECEIVING_DATA,
  DATA_RECEPTION_COMPLETE
} uart_states_t;

#define USART1 ((volatile UART_Struct *) 0x40011400)
#define USART2 ((volatile UART_Struct *) 0x40004400)
#define UART4 ((volatile UART_Struct *) 0x40004C00)
#define UART5 ((volatile UART_Struct *) 0x40005000)

void USART1_IRQHandler(void);
void uart_general_init(volatile UART_Struct *uart_address);

#endif