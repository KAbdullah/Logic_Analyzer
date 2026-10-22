#include <stdint.h>

#ifndef StartAndStop_API_Def
#define StartAndStop_API_Def

typedef struct StartAndStop_API {

  void *context; 

  int (*start_acquisition) (void *context);
  int (*stop_acquisition) (void *context);

} StartAndStop_API;

#endif