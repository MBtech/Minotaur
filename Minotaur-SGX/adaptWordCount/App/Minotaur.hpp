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

#include <boost/algorithm/string.hpp>

#ifdef NATIVE
#include "Enclave.h"
#endif

#ifdef NATIVE
void* Spout (void *arg, std::vector<std::string> file, void (*enclave_func) (InputData* , OutputSpout*));
#else
void* Spout (void *arg, std::vector<std::string> file, sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData* , OutputSpout*));
#endif

#ifdef NATIVE
void* Bolt(void *arg, void (*enclave_func) (InputData* , OutputData*), void (*window_func)(Parallelism*, OutputData*));
#else
void* Bolt(void *arg, sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData* , OutputData*), sgx_status_t (*window_func)(sgx_enclave_id_t, Parallelism*n, OutputData*));
#endif

#ifdef NATIVE
void* Sink(void *arg, void (*enclave_func) (InputData*), void (*window_func)(OutputData*));
#else
void* Sink(void *arg,sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData*), sgx_status_t (*window_func)(sgx_enclave_id_t, OutputData*));
#endif

#ifdef NATIVE
void* Observer(void *arg, void (*enclave_func) (InternalInput*, int, OutputData*), void (*window_func)(OutputData*));
#else
void* Observer(void *arg,sgx_status_t (*enclave_func) (sgx_enclave_id_t, InternalInput*, int, OutputData*), sgx_status_t (*window_func)(sgx_enclave_id_t, OutputData*));
#endif
