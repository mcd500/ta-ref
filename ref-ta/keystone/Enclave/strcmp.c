#include "Enclave_t.h"

#ifdef EDGER_MACROS_H
int strcmp(const char *s1, const char *s2)
{
  return __builtin_strcmp(s1, s2);
}
#endif
