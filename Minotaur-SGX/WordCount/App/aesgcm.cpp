/*
 * Copyright 2012-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/*
 * Simple AES GCM test program, uses the same NIST data used for the FIPS
 * self test but uses the application level EVP APIs.
 */
#include "aesgcm.h"
/* AES-GCM test data from NIST public test vectors */
/*
static const unsigned char gcm_key[] = {
        0xee, 0xbc, 0x1f, 0x57, 0x48, 0x7f, 0x51, 0x92, 0x1c, 0x04, 0x65, 0x66,
        0x5f, 0x8a, 0xe6, 0xd1, 0x65, 0x8b, 0xb2, 0x6d, 0xe6, 0xf8, 0xa0, 0x69,
        0xa3, 0x52, 0x02, 0x93, 0xa5, 0x72, 0x07, 0x8f
};
*/
static const unsigned char gcm_key[] = {
        0xee, 0xbc, 0x1f, 0x57, 0x48, 0x7f, 0x51, 0x92, 0x1c, 0x04, 0x65, 0x66,
        0x5f, 0x8a, 0xe6, 0xd1
};

static const unsigned char gcm_iv[] = {
        0x99, 0xaa, 0x3e, 0x68, 0xed, 0x81, 0x73, 0xa0, 0xee, 0xd0, 0x66, 0x84
};
/*
static const unsigned char gcm_pt[] = {
    0xf5, 0x6e, 0x87, 0x05, 0x5b, 0xc3, 0x2d, 0x0e, 0xeb, 0x31, 0xb2, 0xea,
    0xcc, 0x2b, 0xf2, 0xa5
};*/

static const unsigned char gcm_aad[] = {
        0x4d, 0x23, 0xc3, 0xce, 0xc3, 0x34, 0xb4, 0x9b, 0xdb, 0x37, 0x0c, 0x43,
        0x7f, 0xec, 0x78, 0xde
};
/*
static const unsigned char gcm_ct[] = {
    0xf7, 0x26, 0x44, 0x13, 0xa8, 0x4c, 0x0e, 0x7c, 0xd5, 0x36, 0x86, 0x7e,
    0xb9, 0xf2, 0x17, 0x36
};*/

//unsigned char gcm_tag[] = {
//        0x67, 0xba, 0x05, 0x10, 0x26, 0x2a, 0xe4, 0x87, 0xd7, 0x37, 0xee, 0x62,
//        0x98, 0xf7, 0x7e, 0x0ce
//};

void aes_gcm_encrypt(unsigned char * gcm_pt, size_t len_pt, unsigned char * gcm_ct, unsigned char * gcm_tag)
{
    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen;
    unsigned char outbuf[1024];
    int EVP_CTRL_AEAD_SET_IVLEN = 0x9;
    int EVP_CTRL_AEAD_GET_TAG = 0x10;
    //printf("AES GCM Encrypt:\n");
    //printf("Plaintext:\n");
    //BIO_dump_fp(stdout, reinterpret_cast<const char* >(gcm_pt), len_pt);
    ctx = EVP_CIPHER_CTX_new();
    /* Set cipher type and mode */
    EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    /* Set IV length if default 96 bits is not appropriate */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, sizeof(gcm_iv), NULL);
    /* Initialise key and IV */
    EVP_EncryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);
    /* Zero or more calls to specify any AAD */
    EVP_EncryptUpdate(ctx, NULL, &outlen, gcm_aad, sizeof(gcm_aad));
    /* Encrypt plaintext */
    EVP_EncryptUpdate(ctx, gcm_ct, &outlen, gcm_pt, len_pt);
    /* Output encrypted block */
    //printf("Ciphertext:\n");
    //BIO_dump_fp(stdout, reinterpret_cast<const char*>(gcm_ct), outlen);
    /* Finalise: note get no output for GCM */
    EVP_EncryptFinal_ex(ctx, gcm_ct, &outlen);
    /* Get tag */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, gcm_tag);
	
//    printf("%s \n",gcm_ct);
    /* Output tag */
    //printf("Tag:\n");
    //BIO_dump_fp(stdout, reinterpret_cast<const char*>(gcm_tag), 16);
    EVP_CIPHER_CTX_free(ctx);
}

void aes_gcm_decrypt(unsigned char * gcm_ct, size_t len_ct, unsigned char * gcm_pt, unsigned char * gcm_tag)
{
    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen, rv;
    unsigned char outbuf[1024];
    int EVP_CTRL_AEAD_SET_IVLEN = 0x9;
    int EVP_CTRL_AEAD_SET_TAG = 0x11;
    //printf("AES GCM Derypt:\n");
    //printf("Ciphertext:\n");
    //BIO_dump_fp(stdout, reinterpret_cast<const char*>(gcm_ct), len_ct);
    ctx = EVP_CIPHER_CTX_new();
    /* Select cipher */
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    /* Set IV length, omit for 96 bits */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, sizeof(gcm_iv), NULL);
    /* Specify key and IV */
    EVP_DecryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);
    /* Zero or more calls to specify any AAD */
    EVP_DecryptUpdate(ctx, NULL, &outlen, gcm_aad, sizeof(gcm_aad));
    /* Decrypt plaintext */
    EVP_DecryptUpdate(ctx, gcm_pt, &outlen, gcm_ct, len_ct);
    /* Output decrypted block */
    //printf("Plaintext:\n");
    //BIO_dump_fp(stdout,reinterpret_cast<const char*>(gcm_pt), outlen);
    /* Set expected tag value. */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, sizeof(gcm_tag), (void *)gcm_tag);
    /* Finalise: note get no output for GCM */
    rv = EVP_DecryptFinal_ex(ctx, gcm_pt, &outlen);
    /*
     * Print out return value. If this is not successful authentication
     * failed and plaintext is not trustworthy.
     */
    //printf("Tag Verify %s\n", rv > 0 ? "Successful!" : "Failed!");
    EVP_CIPHER_CTX_free(ctx);
}

//
//int main(int argc, char **argv)
//{
//    unsigned char gcm_pt[] = {
//    0xf5, 0x6e, 0x87, 0x05, 0x5b, 0xc3, 0x2d, 0x0e, 0xeb, 0x31, 0xb2, 0xea,
//    0xcc, 0x2b, 0xf2, 0xa5, 0x5a
//    };
//    unsigned char gcm_ct[sizeof(gcm_pt)/sizeof(gcm_pt[0])];
////    unsigned char gcm_ct[] = {
////    0xf7, 0x26, 0x44, 0x13, 0xa8, 0x4c, 0x0e, 0x7c, 0xd5, 0x36, 0x86, 0x7e,
////    0xb9, 0xf2, 0x17, 0x00
////};
//    //unsigned char gcm_ct[sizeof(gcm_pt)/sizeof(gcm_pt[0])];
//    aes_gcm_encrypt(gcm_pt, sizeof(gcm_pt), gcm_ct, gcm_tag);
//
//
//    aes_gcm_decrypt(gcm_ct, sizeof(gcm_ct), gcm_pt, gcm_tag);
//}
