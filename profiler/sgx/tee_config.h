#include <fcntl.h>
#include "profiler_attrs.h"
#include <stdint.h>

extern uintptr_t __ImageBase[];
static char perf_buffer[PERF_SIZE];

static inline uint64_t tee_rdtscp(uint8_t *id)
{
  uint32_t hi, lo, aux;
  __asm__ volatile("rdtscp" : "=a" (lo), "=d" (hi), "=c" (aux));
  if(id) *id = aux;
  return ((uint64_t)hi << 32) | lo;
}
