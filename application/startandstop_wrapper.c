#include "startandstop_api.h"

int get_signals(StartAndStop_API *api) {
  return api->start_acquisition(api->context);
}

int stop_signals(StartAndStop_API *api) {
  return api->stop_acquisition(api->context);
}