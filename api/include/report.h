
#ifndef _REPORT_H
#define _REPORT_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define MDSIZE  64
#define SIGNATURE_SIZE  64
#define PUBLIC_KEY_SIZE 32
#define ATTEST_DATA_MAXLEN  1024
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/* attestation reports */
struct enclave_report
{
  uint8_t hash[MDSIZE];
  uint64_t data_len;
  uint8_t data[ATTEST_DATA_MAXLEN];
  uint8_t signature[SIGNATURE_SIZE];
};

struct sm_report
{
  uint8_t hash[MDSIZE];
  uint8_t public_key[PUBLIC_KEY_SIZE];
  uint8_t signature[SIGNATURE_SIZE];
};

struct report
{
  struct enclave_report enclave;
  struct sm_report sm;
  uint8_t dev_public_key[PUBLIC_KEY_SIZE];
};

#endif // _REPORT_H
