#include <stdint.h>

#ifndef StartAndStop_API_Def
#define StartAndStop_API_Def

struct StartAndStopConfig;

typedef struct StartAndStop_API {

  struct StartAndStopConfig *context;

  int (*start_acquisition) (struct StartAndStopConfig *context);
  int (*stop_acquisition) (struct StartAndStopConfig *context);

} StartAndStop_API;

#endif