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
#include "Enclave_t.h"  /* print_string */
#include <iterator>
#include <vector>

#include "sgx_tcrypto.h"

using namespace std;

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */

 std::map <std::string, int> count_map;
 std::map <std::string, int> aggregate_map;

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
/*
unsigned char gcm_tag[] = {
        0x67, 0xba, 0x05, 0x10, 0x26, 0x2a, 0xe4, 0x87, 0xd7, 0x37, 0xee, 0x62,
        0x98, 0xf7, 0x7e, 0x0ce
};*/
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

void encrypt(char * line, size_t length, char * p_dst, unsigned char * gcm_tag){

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
    if (SGX_ERROR_MAC_MISMATCH == sgx_status){
//       printf("Mac mismatch");
    }else if (SGX_SUCCESS != sgx_status){
        printf("No sucess");
        }
}


void decrypt(char * line, size_t length, char * p_dst, char * gcm_tag){
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
/*    printf("%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n", t[0],t[1], t[2],
                        t[3], t[4], t[5], t[6], t[7], t[8], t[9], t[10], t[11], t[12], t[13], t[14],t[15]); 
*/    //printf("IV length %d", iv_len);   
    sgx_status_t sgx_status;    
    //printf("%x %x %x", line[0], line[1], line[length-1]);
    sgx_status = sgx_rijndael128GCM_decrypt(reinterpret_cast<const sgx_aes_gcm_128bit_key_t *>(gcm_key), p_src, src_len, reinterpret_cast <uint8_t *>(p_dst), gcm_iv, iv_len, gcm_aad, aad_len, reinterpret_cast<sgx_aes_gcm_128bit_tag_t *>(gcm_tag));
//    printf("%s", (char *) p_dst);
//   aes_gcm_decrypt(p_src, src_len, p_dst, gcm_tag);
    if (SGX_ERROR_MAC_MISMATCH == sgx_status){
//       printf("Mac mismatch");
    }else if (SGX_SUCCESS != sgx_status){
	printf("No sucess");
	}
    delete p_src; 
}

int enclave_shuffle_routing(int j, int n){
    j++;
    j = j% n;
    return j;
}

// What needs to happen within the enclave?
void enclave_spout_execute(int* j, int* n){
	*j = enclave_shuffle_routing(*j,*n);
        //return j;
}

std::vector<std::string> split(const char * str, char c= ' '){
	std::vector<std::string> result;
	do {
		const char * begin = str;
		while (*str != c && *str)
			str++;
		result.push_back(std::string(begin, str));
	}while(0 != *str++);

	return result;
}
void enclave_splitter_execute(char * csmessage, int *slength, char * tag, int *np, StringArray* retmessage, int *retlen, int * nc, MacArray * mac, int *pRoute) {
    //printf("Inside the enclave");
    std::string word;
    int n = *np;
    char p_dst[*slength]; 
    std::string ctsentence(csmessage, *slength);
//    printf("%d", ctsentence.length()); 
    
    decrypt(csmessage,*slength, p_dst, (char *)tag);     
///    printf(tag);
    

    //printf(p_dst);

    std::vector<std::string> s =  split(p_dst,',');
    unsigned int j = 0;
    int count = s.size();
    count = 1; 
    *nc = count;
    int i = 0;
    //Dropoff longitude 18, Dropoff latitude 4, Pickup longitude 16, Pickup latitude 17
    word = s[18]+ "," +s[4]+ "," +s[16]+ "," +s[17];
    for(int k = 0; k<count; k++) {
//	word = s[k];	
	printf("%s", word.c_str());
        std::hash<std::string> hasher;
        long hashed = hasher(word);
        j = abs(hashed % n);

        int len = snprintf(NULL, 0, "%d", j);
        *pRoute = j;
        //*((*retlen)+i) = word.length() + std::to_string(j).length() + 2;
        *(retlen+i) = word.length();
        //retmessage->array[i] = (char *) malloc(*(retlen+i) * sizeof(char));
        //printf(word.c_str());
	unsigned char ret_tag[16];
	char gcm_ct [word.length()];
	encrypt((char * )word.c_str(), word.length(), gcm_ct, ret_tag);
        
	memcpy(mac->array[i], ret_tag, 16);
        //snprintf((char *)mac->array[i], 16, "%s", (char *)ret_tag);		
	memcpy(retmessage->array[i], gcm_ct, *(retlen+i));
        //snprintf(retmessage->array[i], *(retlen+i), "%s", (char *) gcm_ct);

        i = i+1;
    }
}

void enclave_count_execute(char* csmessage, int * slength, char * gcm_tag) {

     char p_dst[*slength];
    std::string ctsentence(csmessage, *slength);
    decrypt(csmessage, *slength, p_dst,(char *)gcm_tag);
    std::string word (p_dst, p_dst+(*slength)); 
    if (count_map.find(word) != count_map.end()) {
        count_map[word.c_str()] += 1;
    } else {
        count_map[word.c_str()] = 1;
    }
    std::map<std::string, int > ::iterator it;
    //printf(word.c_str());
    // Printing the counts
    /*
    for (it = count_map.begin(); it != count_map.end(); it++) {
        std::cout << it->first // string (key)
                << ':'
                << it->second // string's value
                << std::endl;
    }*/
}

std::string find_top_n_keys(std::map <std::string, int> mymap, int n){
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
        printf ("second: %s\n",buffer);
	
//	snprintf(buffer, sizeof(buffer), "%s", (p.first).c_str());
//        printf ("first: %s\n",buffer);

        std::string second(buffer);

        top += p.first + "|" + second + ";";
	printf("%d", count_map[p.first]);
	printf("%s", p.first.c_str());
	printf("%d", (p.first).length());
    }

    return top;
}

void enclave_top_execute(int *np, StringArray* retmessage, int *retlen, int * nc, MacArray * mac, int *pRoute){
    std::string word;

    int n = *np;
    unsigned int j = 0;
//    int count = s.size();
    int count = 1;
    *nc = count;
    int i = 0;

    int top_n = 3; 
    word = find_top_n_keys(count_map, top_n);
    printf(word.c_str());

    for(int k = 0; k<count; k++) {
        std::hash<std::string> hasher;
        long hashed = hasher(word);
	printf("n is %d", n);
	n = 1;
        j = abs(hashed % n);

        int len = snprintf(NULL, 0, "%d", j);
        *pRoute = j;
        //*((*retlen)+i) = word.length() + std::to_string(j).length() + 2;
        *(retlen+i) = word.length();
        //retmessage->array[i] = (char *) malloc(*(retlen+i) * sizeof(char));
        //printf(word.c_str());
        unsigned char ret_tag[16];
        char gcm_ct [word.length()];
        encrypt((char * )word.c_str(), word.length(), gcm_ct, ret_tag);
        memcpy(mac->array[i], ret_tag, 16);
        //snprintf((char *)mac->array[i], 16, "%s", (char *)ret_tag);           
        memcpy(retmessage->array[i], gcm_ct, *(retlen+i));
        //snprintf(retmessage->array[i], *(retlen+i), "%s", (char *) gcm_ct);

        i = i+1;
    }

}

void enclave_aggregate_execute(char* csmessage, int * slength, char * gcm_tag) {

     char p_dst[*slength];
    std::string ctsentence(csmessage, *slength);

    decrypt(csmessage, *slength, p_dst,(char *)gcm_tag);
    
    std::vector<std::string> s =  split(p_dst,';');
    std::string word;

    for(std::vector<std::string>::iterator it = s.begin(); it != s.end(); ++it) {
	std::string c = *it;
	if(c.length()==0){
	    continue;
	}
	std::vector<std::string> kv =  split(c.c_str(),'|');	
    	std::string k = kv.front();

	int v = atoi(kv.back().c_str());

	if (aggregate_map.find(k) != aggregate_map.end()) {
            aggregate_map[k.c_str()] += v;
	} else {
            aggregate_map[k.c_str()] = v;
    	}
    }
	
    int top_n = 3;
    word = find_top_n_keys(aggregate_map, top_n);
    printf(word.c_str());

    // Printing the counts
    /*
    for (it = count_map.begin(); it != count_map.end(); it++) {
        std::cout << it->first // string (key)
                << ':'
                << it->second // string's value
                << std::endl;
    }*/
}

