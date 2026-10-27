#include <stdint.h>

#ifndef StartAndStop_API_Def
#define StartAndStop_API_Def

struct LogicAnalyserConfig;
struct dma_bookkeeper;

typedef struct LogicAnalyser_API {

  struct LogicAnalyserConfig *context;

  int (*start_acquisition) (struct LogicAnalyserConfig *context);
  int (*stop_acquisition) (struct LogicAnalyserConfig *context);
  volatile struct dma_bookkeeper*  (*bookeeping) (struct LogicAnalyserConfig *context);

} LogicAnalyser_API;

#endif