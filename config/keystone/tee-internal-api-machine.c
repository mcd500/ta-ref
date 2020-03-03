#include "eapp_utils.h"
#include "tee-ta-internal.h"

void __attribute__((noreturn)) TEE_Panic(unsigned long code)
{
    for(;;)
      EAPP_RETURN(0xfffe0000 + (code & 0x1ffff));
}
