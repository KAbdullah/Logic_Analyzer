#include "LogicAnalyser_API.h"

#ifndef startandstopwrapper
#define startandstopwrapper

int get_signals(LogicAnalyser_API *api);

int stop_signals(LogicAnalyser_API *api);

volatile struct dma_bookkeeper* valid_samples(LogicAnalyser_API *api);

uint8_t send_uart_data(LogicAnalyser_API *api, uint8_t state, uint32_t counter);

uint32_t receive_command(LogicAnalyser_API *api);

#endif