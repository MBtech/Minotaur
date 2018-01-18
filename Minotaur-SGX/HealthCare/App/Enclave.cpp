/*
 * Copyright (C) 2011-2016 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <string.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include "Enclave.h"
#if defined(SGX) || defined(NOENCRY)
#include "Enclave_t.h"  /* print_string */
#include "sgx_tcrypto.h"
#endif
#include <iterator>
#include <vector>


#include "eutils.hpp"

using namespace std;

/*
 * printf:
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */

std::map <std::string, int> count_map, agg_map;
int prev = 0;
#ifdef SGX
static const unsigned char gcm_key[] = {
    0xee, 0xbc, 0x1f, 0x57, 0x48, 0x7f, 0x51, 0x92, 0x1c, 0x04, 0x65, 0x66,
    0x5f, 0x8a, 0xe6, 0xd1, 0x65, 0x8b, 0xb2, 0x6d, 0xe6, 0xf8, 0xa0, 0x69,
    0xa3, 0x52, 0x02, 0x93, 0xa5, 0x72, 0x07, 0x8f
};

static const unsigned char gcm_iv[] = {
    0x99, 0xaa, 0x3e, 0x68, 0xed, 0x81, 0x73, 0xa0, 0xee, 0xd0, 0x66, 0x84
};

static const unsigned char gcm_aad[] = {
    0x4d, 0x23, 0xc3, 0xce, 0xc3, 0x34, 0xb4, 0x9b, 0xdb, 0x37, 0x0c, 0x43,
    0x7f, 0xec, 0x78, 0xde
};
#endif

#if defined(SGX) || defined(NOENCRY)
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}
#endif

#ifdef SGX
void encrypt(char * line, size_t length, char * p_dst, unsigned char * gcm_tag) {

    uint32_t src_len = length;
    uint8_t*  p_src= reinterpret_cast<uint8_t *>(line);
    uint32_t iv_len = sizeof(gcm_iv)/sizeof(gcm_iv[0]);
    uint32_t aad_len = sizeof(gcm_aad)/sizeof(gcm_aad[0]);

    //printf("IV length %d", iv_len);
    sgx_status_t sgx_status;
    //printf("%x %x %x", gcm_tag[0], gcm_tag[1], gcm_tag[15]);
    //printf("%x %x %x", line[0], line[1], line[length-1]);

    sgx_status = sgx_rijndael128GCM_encrypt(reinterpret_cast<const sgx_aes_gcm_128bit_key_t *>(gcm_key), p_src, src_len, reinterpret_cast <uint8_t *>(p_dst), gcm_iv, iv_len, gcm_aad, aad_len, reinterpret_cast<sgx_aes_gcm_128bit_tag_t *>(gcm_tag));
    //printf("%s", (char *) p_dst);
//   aes_gcm_decrypt(p_src, src_len, p_dst, gcm_tag);
    if (SGX_ERROR_MAC_MISMATCH == sgx_status) {
        printf("Mac mismatch");
    } else if (SGX_SUCCESS != sgx_status) {
        printf("No sucess");
    }
}


void decrypt(char * line, size_t length, char * p_dst, char * gcm_tag) {
    //printf("Inside the decrypt func");
//    printf(line);
    uint32_t src_len = length;

    uint8_t * p_src = (uint8_t *) malloc(length);
    memset(p_src, 0, length);
    memcpy(p_src, line, length);

//    uint8_t*  p_src= reinterpret_cast<uint8_t *>(line);
    uint32_t iv_len = sizeof(gcm_iv)/sizeof(gcm_iv[0]);
    uint32_t aad_len = sizeof(gcm_aad)/sizeof(gcm_aad[0]);

    uint8_t * t = reinterpret_cast<uint8_t *> (gcm_tag);
    sgx_status_t sgx_status;
    //printf("%x %x %x", line[0], line[1], line[length-1]);
    sgx_status = sgx_rijndael128GCM_decrypt(reinterpret_cast<const sgx_aes_gcm_128bit_key_t *>(gcm_key), p_src, src_len, reinterpret_cast <uint8_t *>(p_dst), gcm_iv, iv_len, gcm_aad, aad_len, reinterpret_cast<sgx_aes_gcm_128bit_tag_t *>(gcm_tag));
//    printf("%s", (char *) p_dst);
//   aes_gcm_decrypt(p_src, src_len, p_dst, gcm_tag);
    if (SGX_ERROR_MAC_MISMATCH == sgx_status) {
        printf("Mac mismatch");
    } else if (SGX_SUCCESS != sgx_status) {
        printf("No success");
    }
    delete p_src;
}
#endif

bool vf_check(std::vector<int> data){
     if(data[2]>150 && data[3]==0 && data[4]==0 && data[5]==0 && data[6]==0
	&& data[7]==0 && data[8]==0){
     return true;
	}else{
	return false;
}
}

bool af_check(std::vector<int> data){
     if(data[2]>250 && data[3]==0 && data[4]==0 && data[5]==0 && data[6]==0
        && data[7]>0 && data[8]>0){
     return true;
        }else{
        return false;
}
}

std::vector<int> split(const char * str, char c= ' ') {
    std::vector<int> result;
    do {
        const char * begin = str;
        while (*str != c && *str)
            str++;
        result.push_back(atoi(std::string(begin, str).c_str()));
    } while(0 != *str++);
    return result;
}

void enclave_spout_execute(char* csmessage, Parallelism * n, Routes* routes, Stream * stream) {
    //std::vector<std::string> s = split(csmessage);
    stream->array[0] = 0;
    printf(csmessage);   
    int * r = get_route(csmessage,n->next_parallel[stream->array[0]],0,1);
    //std::copy(r.begin(), r.end(), routes->array[0]);
    memcpy(routes->array[0], r,1*sizeof(int));
    //routes->array[0] = r;
}

void enclave_vf_execute(InputData * input, OutputData * output) {
    std::string word;
    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);
#ifdef SGX
    decrypt(input->message,input->msg_len, p_dst, (char *)input->mac);
#endif
    int count = 0;
    std::vector<int> s = split(p_dst, ',');
    count = s.size();
    unsigned int j = 0;
    int i =0;
    output->total_msgs = count;
    for(int k = 0; k<count; k++) {
        word = std::string(p_dst);
        if(vf_check(s)){
        output->stream[k] = 0;
        }else{
	output->stream[k] = 1;
	}

        int* r = get_route(word, input->next_parallel[output->stream[k]],ROUTE_ALGO, ROUTE_LEN);
        memcpy(output->routes[k], r, ROUTES*sizeof(int));
        free(r);
        output->msg_len[k]  = word.length();
        char gcm_ct [word.length()];
        memcpy(gcm_ct, word.c_str(), word.length());
#ifdef SGX
        unsigned char ret_tag[16];
        encrypt((char * )word.c_str(), word.length(), gcm_ct, ret_tag);
        memcpy(output->mac[k], ret_tag, 16);
#endif
        memcpy(output->message[k], gcm_ct, output->msg_len[k]);
    }
}

void enclave_af_execute(InputData * input, OutputData * output) {
    std::string word;
    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);
#ifdef SGX
    decrypt(input->message,input->msg_len, p_dst, (char *)input->mac);
#endif
    int count = 0;
    std::vector<int> s = split(p_dst, ',');
    count = s.size();
    unsigned int j = 0;
    int i =0;
    output->total_msgs = count;
    for(int k = 0; k<count; k++) {
        word = std::string(p_dst);
        if(af_check(s)){
        output->stream[k] = 0;
        }else{
        output->stream[k] = 1;
        }

        int* r = get_route(word, input->next_parallel[output->stream[k]],ROUTE_ALGO, ROUTE_LEN);
        memcpy(output->routes[k], r, ROUTES*sizeof(int));
        free(r);
        output->msg_len[k]  = word.length();
        char gcm_ct [word.length()];
        memcpy(gcm_ct, word.c_str(), word.length());
#ifdef SGX
        unsigned char ret_tag[16];
        encrypt((char * )word.c_str(), word.length(), gcm_ct, ret_tag);
        memcpy(output->mac[k], ret_tag, 16);
#endif
        memcpy(output->message[k], gcm_ct, output->msg_len[k]);
    }
}


void enclave_emerg_execute(InputData * input) {

    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);

#ifdef SGX
    decrypt(input->message, input->msg_len, p_dst,(char *)input->mac);
#endif
    std::vector<int> s = split(p_dst);
    printf("Emergency for user %d", s[0]);
    printf(p_dst);
}

void enclave_other_execute(InputData * input) {

    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);

#ifdef SGX
    decrypt(input->message, input->msg_len, p_dst,(char *)input->mac);
#endif
    std::vector<int> s = split(p_dst);
    printf(p_dst);
}

void dummy_window_func(Parallelism *n , OutputData* output){
  output->total_msgs = 0;
}
void dummy_window_sink(OutputData* output) {
    output->total_msgs = 0;
}
