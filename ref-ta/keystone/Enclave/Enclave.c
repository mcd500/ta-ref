#include "tools.h"

//#include "hacks.h"

void gp_random_test();
void gp_ree_time_test();
void gp_trusted_time_test();
void gp_secure_storage_test();
void gp_message_digest_test();
void gp_symmetric_key_enc_verify_test();
void gp_asymmetric_key_sign_test();

void gp_symmetric_key_gcm_verify_test();

int main(void) {
  //edge_init();
  magic_random_init();

  printf("gp ecall_ta_main() start\n");

  gp_random_test();

  gp_ree_time_test();

  gp_trusted_time_test();

  gp_secure_storage_test();

  gp_message_digest_test();

  gp_symmetric_key_enc_verify_test();

  gp_symmetric_key_gcm_verify_test();
  // symmetric_key_dec_verify_test();

  gp_asymmetric_key_sign_test();

  // asymmetric_key_verify_test();
#ifdef NOT_DONE
#endif

  printf("gp ecall_ta_main() end\n");

  return 0;
}
