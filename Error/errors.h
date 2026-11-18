#ifndef ERROR
#define ERROR

typedef enum SYSTEM_ERRORS{
  ERROR_FREE = 100U,
  DMA_TRANSFER_ERROR = 101U,
  DMA_FIFO_ERROR = 102U,
  DMA_DIRECT_MODE_ERROR = 103U,
} SYSTEM_ERRORS;

//Extern only promises that this variable will be declared exactly this way: volatile enum SYSTEM_ERRORS current_error_state = whatever you want here;
extern volatile enum SYSTEM_ERRORS current_error_state;

void handle_transfer_error();

#endif