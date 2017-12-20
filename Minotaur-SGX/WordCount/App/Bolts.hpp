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
#include <functional>
#include <stdlib.h>
#include <map>
#include <fstream>
#include <vector>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "App.h"

#include "user_types.h"
#if defined(SGX) || defined(NOENCRY)
#include "Enclave_u.h"
#include "sgx_urts.h"
#endif

#include "aesgcm.h"
#include "message.hpp"
#include <msgpack.hpp>

#include "TimedBuffer.hpp"

#ifdef NATIVE
#include "Executors.hpp"
#endif

#include <boost/algorithm/string.hpp>

void encrypt(char * line, size_t len_pt, unsigned char * gcm_ct, unsigned char * gcm_tag);

void* spout (void *arg);

void* splitter(void* arg);
void* count(void* arg);
int func_main(int argc, char** argv);

