#include "gp_test.h"

int main(void) {
    gp_random_test();

    gp_ree_time_test();

    gp_trusted_time_test();

    gp_secure_storage_test();

    gp_message_digest_test();

    gp_symmetric_key_enc_verify_test();

    gp_symmetric_key_gcm_verify_test();

    gp_asymmetric_key_sign_test();
}
