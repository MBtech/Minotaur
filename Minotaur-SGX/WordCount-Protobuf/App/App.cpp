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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <time.h>
#include "zhelpers.hpp"
#include "zmq.hpp"
#include "utils.h"
#include <functional>
#include <stdlib.h>
#include <map>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

#include "aesgcm.h"
#include "message.pb.h"

cbuffer_t buffer;
cbuffer_t *p1 = &buffer;
cbuffer_t **p = &p1;

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;
long ITERATIONS = 10000;
static const uint8_t gcm_key[] = {
    0xee, 0xbc, 0x1f, 0x57, 0x48, 0x7f, 0x51, 0x92, 0x1c, 0x04, 0x65, 0x66,
    0x5f, 0x8a, 0xe6, 0xd1, 0x65, 0x8b, 0xb2, 0x6d, 0xe6, 0xf8, 0xa0, 0x69,
    0xa3, 0x52, 0x02, 0x93, 0xa5, 0x72, 0x07, 0x8f
};

static const uint8_t gcm_iv[] = {
    0x99, 0xaa, 0x3e, 0x68, 0xed, 0x81, 0x73, 0xa0, 0xee, 0xd0, 0x66, 0x84
};

static const uint8_t gcm_aad[] = {
    0x4d, 0x23, 0xc3, 0xce, 0xc3, 0x34, 0xb4, 0x9b, 0xdb, 0x37, 0x0c, 0x43,
    0x7f, 0xec, 0x78, 0xde
};


typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
        printf("Error: Unexpected error occurred.\n");
}

/* Initialize the enclave:
 *   Step 1: try to retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void)
{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    
    /* Step 1: try to retrieve the launch token saved by last transaction 
     *         if there is no token, then create a new one.
     */
    /* try to get the token saved in $HOME */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    
    if (home_dir != NULL && 
        (strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
        /* compose the token path */
        strncpy(token_path, home_dir, strlen(home_dir));
        strncat(token_path, "/", strlen("/"));
        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
    } else {
        /* if token path is too long or $HOME is NULL */
        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
    }

    FILE *fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
        printf("Token: %s \n",token);
        printf("Token path: %s \n",token_path);
    }
    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        if (fp != NULL) fclose(fp);
        return -1;
    }

    /* Step 3: save the launch token if it is updated */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) fclose(fp);
        return 0;
    }

    /* reopen the file with write capablity */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL) return 0;
    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t))
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
    fclose(fp);
    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s\n", str);
}


void encrypt(char * line, size_t len_pt, unsigned char * gcm_ct, unsigned char * gcm_tag){
    unsigned char * gcm_pt = reinterpret_cast<unsigned char *>(line);

    aes_gcm_encrypt(gcm_pt, len_pt, gcm_ct, gcm_tag);
}

void* spout (void *arg, std::string ip, int port)
{
    zmq::context_t context (1);
    struct Arguments * param = (Arguments*) arg;
    zmq::socket_t sender = shuffle_sender_conn(param, context, ip, port);
    std::cout << "Sending tasks to workers…\n" << std::endl;
    zmq::message_t message(2);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));
    
    std::string ptsentence ("Hello is it me you are looking for?");
    unsigned char gcm_ct [ptsentence.length()/sizeof(char)];
    uint8_t gcm_tag [16];

    encrypt(strdup(ptsentence.c_str()), ptsentence.length(), gcm_ct, gcm_tag);
    std::string ctsentence = (char *)gcm_ct;
    std::string mac = (char*) gcm_tag;

    // Create the protobuf
    stream::Message sendmessage;
    sendmessage.set_value(ctsentence);
    sendmessage.set_gcm_tag(mac);
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);    
    sendmessage.set_time(tv.tv_nsec);

    std::string sentence;
    sendmessage.SerializeToString(&sentence);

    int n = param->next_stage;
    int j = 0;

    while(1){

        enclave_spout_execute(global_eid,&j,&n);

        int len = sentence.length()+std::to_string(j).length()+1;
        message.rebuild(len);

        sprintf((char *)message.data(),"%d %s",j,sentence.c_str());
        sender.send(message);
        sleep(1);
    }
    return NULL;
}

void* splitter(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
        std::vector<std::string> receiverIP, std::vector<int> receiverPort) {
    struct Arguments * param = (Arguments*) arg;
    zmq::context_t context(1);
    //zmq::context_t context = zmq_ctx_new();
    zmq::socket_t sender = key_sender_conn(param, context, senderIP, senderPort);
    std::cout << "Splitter: Received the sender socket " << std::endl;

    zmq::socket_t receiver = shuffle_receiver_conn(param, context, receiverIP, receiverPort);

    std::cout << "Starting the splitter worker " << std::endl;

    std::cout << "Reading messages, splitter" << std::endl;
    //  Process tasks forever
    while (1) {

        zmq::message_t message;

        std::string word;
        receiver.recv(&message);
        std::string smessage(static_cast<char*> (message.data()), message.size());

        stream::Message recvmessage;
       recvmessage.ParseFromString(smessage);
	std::string value = recvmessage.value();
        std::string gcm_tag = recvmessage.gcm_tag();
        long time = recvmessage.time();

        //std::cout << smessage << std::endl;

	int n = param -> next_stage;
        int * nPointer = &n;

        StringArray *retmessage = (StringArray *) malloc(sizeof(StringArray));
        std::cout << "Just a check" << std::endl;
        int retlen_a[20];
        int * retlen = (int *) malloc(sizeof(int));
	
	MacArray * mac = (MacArray* ) malloc(sizeof(MacArray));

        std::cout << smessage.c_str() << std::endl;
	int route = 0;
        int *pRoute = &route;
        enclave_splitter_execute(global_eid, (char*)value.c_str(), (char *)gcm_tag.c_str(),nPointer, retmessage, retlen_a, retlen, mac, pRoute);

        std::cout << "Total Message " << *retlen << std::endl;
        for (int k = 0; k < *retlen; k++) {
	  //   Create the protobuf
           stream::Message sendmessage;
           sendmessage.set_value(std::string(retmessage->array[k]));
           sendmessage.set_gcm_tag((const char *)mac->array[k]);
           sendmessage.set_time(time);

           std::string sentence;
           sendmessage.SerializeToString(&sentence);
           
            std::cout << "Length of word " << retlen_a[k]<<std::endl;
	    int len = snprintf(NULL, 0, "%d", *pRoute);
            message.rebuild(sentence.length()+len+2);

            std::cout << (retmessage)->array[k] << std::endl;
            snprintf((char *) message.data(), retlen_a[k], "%d %s", *pRoute, sentence.c_str());
            sender.send(message);
        }

    
    return NULL;
}
}

void* count(void *arg, std::string receiverIP, int port)
{
    struct Arguments * param = (Arguments*) arg;
    zmq::context_t context(1);
    zmq::socket_t receiver = key_receiver_conn(param, context, receiverIP, port);

    std::cout << "Starting the count worker " << std::endl;
    //  Process tasks forever
    while(1) {
        zmq::message_t message;
        receiver.recv(&message);
        std::cout << "Message Received " << std::endl;
        std::string smessage(static_cast<char*> (message.data()), message.size());
        
        int pos = smessage.find_first_of(' ');
        std::string mainmessage = smessage.substr(pos+1); 
        stream::Message recvmessage;
        recvmessage.ParseFromString(mainmessage);
        std::string value = recvmessage.value();
        std::string gcm_tag = recvmessage.gcm_tag();
        long time = recvmessage.time();
        
        enclave_count_execute(global_eid,value.c_str(), gcm_tag.c_str());

        struct timespec tv;
        clock_gettime(CLOCK_REALTIME, &tv);
        std::cout << tv.tv_nsec - time<<std::endl;
    }
    return NULL;
}


int func_main(int argc, char** argv){
    const int count_threads = 1;
    const int split_threads = 1;
    const int spout_threads = 1;

    pthread_t spout_t[spout_threads];
    pthread_t split_t[split_threads];
    pthread_t count_t[count_threads];

    std::cout << argv[1] << std::endl;
    if(strcmp(argv[1], "spout")==0){
        std::cout << "Starting spout" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = split_threads;
        arg->prev_stage = 0;
        spout((void*) arg, argv[3], atoi(argv[4]));
    }
    if(strcmp(argv[1], "splitter")==0){
        std::cout << "Starting splitter" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }

        splitter((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "count")==0){
        std::cout << "Starting count" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = 0;
        arg->prev_stage = split_threads;
        count((void*)arg, argv[3], atoi(argv[4]));
    }

    return 0;
}

int SGX_CDECL main(int argc, char *argv[])
{
    printf("starting the program \n");
    (void)(argc);
    (void)(argv);
    
    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }

    printf("Starting the threads \n");    
    func_main(argc, argv);
    /* Utilize edger8r attributes */
    edger8r_array_attributes();
    edger8r_pointer_attributes();
    edger8r_type_attributes();
    edger8r_function_attributes();
    
    /* Utilize trusted libraries */
    ecall_libc_functions();
    ecall_libcxx_functions();
    ecall_thread_functions();

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    
    printf("Info: SampleEnclave successfully returned.\n");

    printf("Enter a character before exit ...\n");
//    getchar();
    return 0;
}

