#include "eapp_utils.h"
#include "tee-ta-internal.h"

/**
 * TEE_Panic() - Raises a Panic in the Trusted Application instance.
 * 
 * TEE_Panic When a Trusted Application calls the TEE_Panic function, The current 
 * instance SHALL be destroyed and all the resources openedby the instance SHALL 
 * be reclaimed. All sessions opened from the panicking instance on another TA 
 * shall be gracefully closed and all cryptographic objects and operations shall 
 * be closed properly.
 * 
 * @param code        An informative panic code defined by the TA.
 * 
 * @return            panic code will be returned.
 */
void __attribute__((noreturn)) TEE_Panic(unsigned long code)
{
    for(;;)
      EAPP_RETURN(0xfffe0000 + (code & 0x1ffff));
}
