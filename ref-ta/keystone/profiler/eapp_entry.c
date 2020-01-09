#include "eapp_utils.h"
#include "profiler.h"

int _main(void);

void EAPP_ENTRY eapp_entry(){
  __profiler_map_info();
  int ret = _main();
  __profiler_unmap_info();

  EAPP_RETURN(ret);
}
