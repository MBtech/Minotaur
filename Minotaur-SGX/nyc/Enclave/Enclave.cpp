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

std::map <std::string, int> count_map;
std::map<std::string, int> agg_map;
std::map<std::string, int> final_map;
int TOP_N=3;
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

std::vector<std::string> split(const char * str, int len, char c= ' ') {
    std::vector<std::string> result;

    do {
        const char * begin = str;
        while (*str != c && *str && len>0) {
            str++;
            len --;
        }
        result.push_back(std::string(begin, str));
        str++;
        len--;
    } while(len>0);
    //} while(0 != *str++);
    // Remove the last character
    //result.back().pop_back();
    return result;
}

std::string find_top_n_keys(std::map <std::string, int> mymap, int n) {
    std::vector<std::pair<std::string, int>> top_n(n);
    std::partial_sort_copy(mymap.begin(),
                           mymap.end(),
                           top_n.begin(),
                           top_n.end(),
                           [](std::pair<const std::string, int> const& l,
                              std::pair<const std::string, int> const& r)
    {
        return l.second > r.second;
    });
    std::string top;
    for (auto const& p: top_n)
    {
        char buffer [33];
        snprintf(buffer, sizeof(buffer), "%d", p.second);
        std::string second(buffer);
        top += p.first + "|" + second + ";";
    }

    return top;
}

void enclave_spout_execute(InputData *input,  OutputSpout * output) {
    //std::vector<std::string> s = split(csmessage);
    std::string word;
    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);
#ifdef SGX
    decrypt(input->message,input->msg_len, p_dst, (char *)input->mac);
#endif
    int valid = 0;
    int stream_id = 0;
    int * r = get_route(p_dst, input->next_parallel[stream_id],0,1, &valid);
    //std::copy(r.begin(), r.end(), routes->array[0]);
    //memcpy(routes->array[0], r,1*sizeof(int));
    for(int i=0; i<ROUTES; i++) {
        output->routes[i] = r[i];
        output->stream[i] = stream_id;
        output->total_msgs++;
        output->msg_len[i]  = input->msg_len;
        char gcm_ct [input->msg_len];
        memcpy(gcm_ct, p_dst, input->msg_len);
#ifdef SGX
        unsigned char ret_tag[16];
        encrypt(p_dst, input->msg_len, gcm_ct, ret_tag);
        memcpy(output->mac[i], ret_tag, 16);
#endif
        memcpy(output->message[i], gcm_ct, output->msg_len[i]);
    }
	free(r);
}


void enclave_filter_execute(InputData * input, OutputData * output) {
    std::string word;
    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);
#ifdef SGX
    decrypt(input->message,input->msg_len, p_dst, (char *)input->mac);
#endif
    int count = 0;
    std::vector<std::string> s = split(p_dst, input->msg_len,',');
    unsigned int j = 0;
    int k =0;
    output->total_msgs = 0;
    int valid = 0;

    int length = snprintf( NULL, 0, "%d", 1 );
    char* str = (char *)malloc( length + 1 );
    snprintf( str, length + 1, "%d", 1 );

    word = s[18]+ "," +s[4]+ "," +s[16]+ "," +s[17]+ " " + std::string(str);
    int stream_id = 0;
    int* r = get_route(word, input->next_parallel[stream_id], ROUTE_ALGO, ROUTE_LEN, &valid);
    word = word + " " + std::string(str);
    free(str);
    for(int i=0; i<ROUTES; i++) {
        std::string new_word;
        if(i==valid) {
            new_word = append_valid(word, 1);
        } else {
            new_word = append_valid(word, 0);
        }
        output->routes[i] = r[i];
        output->stream[i] = stream_id;
        output->total_msgs++;
        output->msg_len[i]  = new_word.length();
        char gcm_ct [new_word.length()];
        memcpy(gcm_ct, new_word.c_str(), new_word.length());
#ifdef SGX
        unsigned char ret_tag[16];
        encrypt((char * )new_word.c_str(), new_word.length(), gcm_ct, ret_tag);
        memcpy(output->mac[i], ret_tag, 16);
#endif
        memcpy(output->message[i], gcm_ct, output->msg_len[i]);
    }
    free(r);
}

void enclave_aggregate_execute(InputData * input, OutputData * output) {
    std::string word;
    //int n = input -> next_parallel;
    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);
#ifdef SGX
    decrypt(input->message,input->msg_len, p_dst, (char *)input->mac);
#endif
    int count = 0;
    unsigned int j = 0;
    int i =0;
    std::vector<std::string>s = split(p_dst, input->msg_len);
    word = s[0];
    int c = atoi(s[1].c_str());
    // std::string word (p_dst, p_dst+(input->msg_len));
    if (agg_map.find(word) != agg_map.end()) {
        agg_map[word] += c;
    } else {
        agg_map[word] = c;
    }
}

void aggregate_window(Parallelism * n , OutputData * output) {
    std::map<std::string, int>::iterator it = agg_map.begin();
    std::string word;
    output->total_msgs = 0;
    int k =0;
    for(std::advance(it, prev); it!=agg_map.end() && output->total_msgs <MAX_WORD_IN_SENTENCE; ++it ) {
        output->total_msgs += 1;
        output->stream[k] = 0;
        int length = snprintf( NULL, 0, "%d", it->second);
        char* str = (char *)malloc( length + 1 );
        snprintf( str, length + 1, "%d", it->second );
        word = it->first + " "+ std::string(str);
        free(str);
	int valid = 0;
        int* r = get_route(it->first, n->next_parallel[output->stream[k]],  1, 1, &valid);
        /*long hashed = hashit(it->first);
        int route  = abs((hashed%1000) % n->next_parallel[output->stream[k]]);
        printf("%lu , %d", hashed, route);*/
        output->routes[k] = r[0];
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
        k+=1;
    }
    prev += output->total_msgs;
    if(it==agg_map.end()) {
        prev  = 0;
        agg_map.clear();
    }
}

void enclave_count_execute(InputData * input, OutputData * output) {

    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);

#ifdef SGX
    decrypt(input->message, input->msg_len, p_dst,(char *)input->mac);
#endif
    std::vector<std::string> s = split(p_dst, input->msg_len);
    std::string word = s[0];
    double temp = ::atof(word.c_str());
    int length = snprintf( NULL, 0, "%.3f", temp);
    char* str = (char *)malloc( length + 1 );
    snprintf( str, length + 1, "%.3f", temp );
    int c = atoi(s[1].c_str());
    // std::string word (p_dst, p_dst+(input->msg_len));
    printf("%s",p_dst);
    if(((s.size()>2) && (atoi(s[2].c_str())==1)) || s.size()<=2) {
        if (count_map.find(str) != count_map.end()) {
            count_map[str] += c;
        } else {
            count_map[str] = c;
        }
    } else {
        printf("Not Valid. Fake Packet");
    }
    free(str);
    /*
        #ifdef NATIVE
        printf("%s\n",word.c_str());
        #else
        printf(word.c_str());
        #endif
    */
}

void dummy_window_func(Parallelism *n , OutputData* output) {
    output->total_msgs = 0;
}
void dummy_window_sink(OutputData* output) {
    output->total_msgs = 0;
}

void count_window(Parallelism* n , OutputData * output) {
    std::string word;
    output->total_msgs = 1;
    int k =0;
    if(count_map.size()==0) {
        output->total_msgs = 0;
    } else {
        printf("Size of count map: %d", count_map.size());
        word = find_top_n_keys(count_map, TOP_N);
        output->stream[k]=0;
	int valid = 0;
        int* r = get_route(word, n->next_parallel[output->stream[k]], 1,1, &valid);
        output->routes[k] = r[0];
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

        count_map.clear();
    }
}

void enclave_final_execute(InputData * input) {

    char p_dst[input->msg_len];
    memcpy(p_dst, input->message, input->msg_len);

#ifdef SGX
    decrypt(input->message, input->msg_len, p_dst,(char *)input->mac);
#endif
    std::vector<std::string> s =  split(p_dst,input->msg_len,';');
    std::string word;

    for(std::vector<std::string>::iterator it = s.begin(); it != s.end(); ++it) {
        std::string c = *it;
        if(c.length()==0) {
            continue;
        }
        std::vector<std::string> kv =  split(c.c_str(),c.length(),'|');
        std::string k = kv.front();

        int v = atoi(kv.back().c_str());

        if (final_map.find(k) != final_map.end()) {
            final_map[k.c_str()] += v;
        } else {
            final_map[k.c_str()] = v;
        }
    }

    word = find_top_n_keys(final_map, TOP_N);
    printf(word.c_str());
    /*
        #ifdef NATIVE
        printf("%s\n",word.c_str());
        #else
        printf(word.c_str());
        #endif
    */
}

// TODO: ADD final window function
