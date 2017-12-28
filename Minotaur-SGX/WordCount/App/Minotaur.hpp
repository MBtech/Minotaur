#include <stdio.h>
#include <string.h>
#include <assert.h>
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

#ifdef NATIVE
void* Spout (void *arg, void (*enclave_func) (char* , int* , int*, Routes*));
#else
void* Spout (void *arg, sgx_status_t (*enclave_func) (sgx_enclave_id_t, char* , int* , int*, Routes*));
#endif

#ifdef NATIVE
void* Bolt(void *arg, void (*enclave_func) (InputData* , OutputData*));
#else
void* Bolt(void *arg, sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData* , OutputData*));
#endif

#ifdef NATIVE
void* Sink(void *arg, void (*enclave_func) (InputData*));
#else
void* Sink(void *arg,sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData*));
#endif
