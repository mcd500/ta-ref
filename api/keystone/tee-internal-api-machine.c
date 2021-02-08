#include "eapp_utils.h"
#include "tee-ta-internal.h"

/**
 * TEE_Panic() - Raises a panic in the Trusted Application instance.
 * 
 * When a Trusted Application calls the TEE_Panic function, the current 
 * instance shall be destroyed and all the resources opened by the instance shall 
 * be reclaimed. All sessions opened from the panicking instance on another TA 
 * shall be gracefully closed and all cryptographic objects and operations shall 
 * be closed properly.
 * 
 * @param code		An informative panic code defined by the TA.
 * 
 * @return		panic code will be returned.
 */
void __attribute__((noreturn)) TEE_Panic(unsigned long code)
{
    for(;;)
      EAPP_RETURN(0xfffe0000 + (code & 0x1ffff));
}
