#include "LogicAnalyser_API.h"
#include "LogicAnalyser_Wrapper.h"


int get_signals(LogicAnalyser_API *api) {
  return api->start_acquisition(api->context);
}

int stop_signals(LogicAnalyser_API *api) {
  return api->stop_acquisition(api->context);
}

volatile struct dma_bookkeeper* valid_samples(LogicAnalyser_API *api) {
  return api->bookeeping(api->context);
}

uint8_t send_uart_data(LogicAnalyser_API *api, uint8_t state, uint32_t counter) {
  return api->send_uart_data(state, counter);
}

uint32_t receive_command(LogicAnalyser_API *api) {
  return api->receive_uart_data(api->context);
}