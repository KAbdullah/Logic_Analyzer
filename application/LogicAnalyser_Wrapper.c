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