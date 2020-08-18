// Adding strcmp for lib/mbed-crypto/library/cipher.c when keedger8r is used.
// keyedger adds strcmp and several other miscellaneous functions in
// keyedge/target/include/common_header.h, ATM. It may be temporary.
int strcmp(const char *s1, const char *s2)
{
  return __builtin_strcmp(s1, s2);
}
