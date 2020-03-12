// empty
#pragma once
#define GP_ASSERT(rv,msg) \
  do { \
    if ((rv)) { \
      printf("%s:%d %s (%x)\n", __FUNCTION__, __LINE__, (msg), rv); return; \
    } \
  } while(0)
