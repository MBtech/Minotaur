#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

void aes_gcm_encrypt(unsigned char * gcm_pt, size_t len_pt, unsigned char * gcm_ct, unsigned char * gcm_tag);
void aes_gcm_decrypt(unsigned char * gcm_ct, size_t len_ct, unsigned char * gcm_pt, unsigned char * gcm_tag);
