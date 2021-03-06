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


#include <stdint.h>
/* User defined types */
#ifndef _USER_TYPESH_
#define _USER_TYPESH_


#define LOOPS_PER_THREAD 500
#define BUFFER 10
#define GCM_TAG_LEN 16
#define MAX_WORD_LEN 20
#define MAX_WORD_IN_SENTENCE 30
#define MAX_TUPLE_LEN 100
#define BUFFER_TIMEOUT 200
#define MAX_ROUTES 20
#define SLEEP 100
#define TOTAL_STREAMS 1

#define ROUTE_LEN 4
#define ROUTE_ALGO 4
#define ROUTES 1 
//#define NATIVE
#define SGX
//#define NOENCRY

struct StringArray{
    char array[MAX_WORD_LEN][MAX_WORD_IN_SENTENCE];
};
typedef struct StringArray StringArray;

struct MacArray{
     uint8_t array[GCM_TAG_LEN][MAX_WORD_IN_SENTENCE];
};
typedef struct MacArray MacArray;

struct Routes{
        int array[MAX_WORD_IN_SENTENCE*ROUTES];
};

struct Stream{
        int array[MAX_WORD_IN_SENTENCE];
};

struct InputData{
        char message[MAX_TUPLE_LEN];
        int msg_len;
#ifdef SGX
        uint8_t mac[GCM_TAG_LEN];
#endif
        int next_parallel[TOTAL_STREAMS];
        int source;
};

struct OutputSpout{
	 char message[ROUTES][MAX_TUPLE_LEN];
        int msg_len[ROUTES];
        int total_msgs;
#ifdef SGX
        uint8_t mac[ROUTES][GCM_TAG_LEN];
#endif
        int routes[ROUTES];
        int stream[ROUTES];
};

struct OutputData{
        char message[MAX_WORD_IN_SENTENCE*ROUTES][MAX_WORD_LEN];
        int msg_len[MAX_WORD_IN_SENTENCE*ROUTES];
        int total_msgs;
#ifdef SGX
        uint8_t mac[MAX_WORD_IN_SENTENCE*ROUTES][GCM_TAG_LEN];
#endif
        int routes[MAX_WORD_IN_SENTENCE*ROUTES];
        int stream[MAX_WORD_IN_SENTENCE*ROUTES];
};


// Fix this Data structure
struct InternalOutput{
        char message[1][10000];
        int msg_len[1];
#ifdef SGX
        uint8_t mac[1][GCM_TAG_LEN];
#endif
        int routes[1];
        int stream[1];
};

struct InternalInput{
        char message[10000];
        int msg_len;
#ifdef SGX
        uint8_t mac[GCM_TAG_LEN];
#endif
};

struct Parallelism{
	int next_parallel[TOTAL_STREAMS];
};
typedef struct Parallelism Parallelism;


typedef void *buffer_t;
typedef int array_t[10];
typedef int word_len[MAX_WORD_IN_SENTENCE];


typedef struct Routes Routes;
typedef struct Stream Stream;

typedef struct InputData InputData;

typedef struct OutputData OutputData;

typedef struct OutputSpout OutputSpout;

typedef struct InternalOutput InternalOutput; 

typedef struct InternalInput InternalInput;
#endif
