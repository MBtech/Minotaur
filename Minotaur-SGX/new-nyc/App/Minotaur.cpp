#include "Minotaur.hpp"

#ifdef SGX
void encrypt(char * line, size_t len_pt, unsigned char * gcm_ct, unsigned char * gcm_tag) {
    unsigned char * gcm_pt = reinterpret_cast<unsigned char *>(line);
    aes_gcm_encrypt(gcm_pt, len_pt, gcm_ct, gcm_tag);
}
#endif

#ifdef NATIVE
void* Spout (void *arg, void (*enclave_func) (char* , int* , int*, Routes*, Stream*))
#else
void* Spout (void *arg, sgx_status_t (*enclave_func) (sgx_enclave_id_t, char* , int* , int*, Routes*, Stream*))
#endif
{
    zmq::context_t * context;
    zmq::socket_t * sender, * receiver;
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int n = 0, m=0;
    zmq_init(arg, context, socks, &n, &m);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    std::ifstream datafile("nyc-taxi.csv");
    //std::string ptsentence ("Hello is it me you are looking for?");
    std::string ptsentence;
    int j = 0;
    std::vector<zmq::socket_t*> sockets;
    sockets.push_back(socks->sender);
    TimedBuffer s_buff(context,sockets, BUFFER_TIMEOUT);
    sleep(10);
    while(1) {
        while(std::getline(datafile, ptsentence)) {
            struct timespec tv;
            clock_gettime(CLOCK_REALTIME, &tv);
            boost::trim(ptsentence);
            //ptsentence = "Hark. They are speaking";
#ifdef SGX
            unsigned char gcm_ct [ptsentence.length()];
            uint8_t gcm_tag [GCM_TAG_LEN];

            encrypt(strdup(ptsentence.c_str()), ptsentence.length(), gcm_ct, gcm_tag);
            std::string ctsentence((char *)gcm_ct, (int)ptsentence.length());
            std::string mac((char*) gcm_tag, GCM_TAG_LEN);
#endif

            Routes * routes = (Routes* ) malloc(sizeof(Routes));
            Stream * stream = (Stream *) malloc(sizeof(Stream));
#ifdef NATIVE
            enclave_func((char*) ptsentence.c_str(),&n,&j, routes, stream);
#else
            enclave_func(global_eid,(char*) ptsentence.c_str(),&n,&j, routes, stream);
#endif
            for(int i=0; i<ROUTES; i++) {
                std::cout << routes->array[0][i] << std::endl;
                std::cout << ptsentence << "  "<<  ptsentence.length()<<std::endl;
#ifdef SGX
                s_buff.add_msg(stream->array[0], routes->array[0][i], ctsentence, mac, tv.tv_sec, tv.tv_nsec);
#else
                s_buff.add_msg(stream->array[0], routes->array[0][i], ptsentence, tv.tv_sec, tv.tv_nsec);
#endif
                s_buff.check_and_send(false);
            }
            usleep(SLEEP);
        }
        datafile.clear();
        datafile.seekg(0);
    }
    return NULL;
}

#ifdef NATIVE
void* Bolt(void *arg, void (*enclave_func) (InputData* , OutputData*), void(*window_func)(int *, OutputData*)) {
#else
void* Bolt(void *arg, sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData* , OutputData*),
           sgx_status_t (*window_func)(sgx_enclave_id_t, int*n, OutputData*) ) {
#endif
    Arguments * param = (Arguments*) arg;
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    unsigned long beginTime = tv.tv_sec;
    unsigned long currTime = tv.tv_sec;

    zmq::context_t * context;
    zmq::socket_t * sender, * receiver;
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int n = 0, m=0;
    zmq_init(arg, context, socks, &n, &m);
    std::vector<zmq::socket_t*> sockets;
    sockets.push_back(socks->sender);
    TimedBuffer s_buff(context,sockets, BUFFER_TIMEOUT);
    //  Process tasks forever
    while (1) {
        zmq::message_t message;

        std::string word;
        std::string topic = s_recv(*socks->receiver);
        socks->receiver->recv(&message);

        Message msg;
        msgpack::unpacked unpacked_body;
        msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char *> (message.data()), message.size());

        msgpack::object obj = oh.get();
        obj.convert(msg);

        std::vector<long> timeNSec = msg.timeNSec;
        std::vector<long> timeSec = msg.timeSec;

        InputData * input = (InputData* ) malloc(sizeof(InputData));
        OutputData * output = (OutputData* ) malloc(sizeof(OutputData));
        std::vector<std::string> msg_buffer = msg.value;
#ifdef SGX
        std::vector<std::string> mac_buffer = msg.gcm_tag;
#endif

        std::vector<std::string>::iterator it, it1;
        std::vector<long>::iterator it2, it3;
        input->next_parallel = n;
#ifdef SGX
        it1 = mac_buffer.begin();
#endif
        for(it = msg_buffer.begin(), it2=timeSec.begin(), it3=timeNSec.begin(); it != msg_buffer.end(); ++it, ++it2, ++it3) {
            clock_gettime(CLOCK_REALTIME, &tv);
            long latency = calLatency(tv.tv_sec, tv.tv_nsec, *it2, *it3);
            std::cout << "Network Latency: " << latency<<std::endl;

            std::string val = *it;
            input->msg_len = val.length();
            std::copy(val.begin(), val.end(), input->message);
#ifdef SGX
            std::string tag = *it1;
            std::copy(tag.begin(), tag.end(), input->mac);
#endif
#ifdef NATIVE
            enclave_func(input, output);
#else
            enclave_func(global_eid, input, output);
#endif
            //std::cout << "Total message: " << output->total_msgs << std::endl;
            for (int k = 0; k < output->total_msgs; k++) {
#ifdef SGX
                s_buff.add_msg(output->stream[k],output->routes[k][0], std::string(output->message[k], output->msg_len[k]),std::string((char*) output->mac[k], GCM_TAG_LEN), tv.tv_sec, tv.tv_nsec);
#else
                s_buff.add_msg(output->stream[k], output->routes[k][0], std::string(output->message[k], output->msg_len[k]), tv.tv_sec, tv.tv_nsec);
#endif
                s_buff.check_and_send(false);
            }
            if(output->total_msgs ==0){
		struct timespec t; 
clock_gettime(CLOCK_REALTIME, &t);
	                    long latency = calLatency(t.tv_sec, t.tv_nsec, tv.tv_sec, tv.tv_nsec);
            std::cout << "Processing Latency: " << latency<<std::endl;
	}
#ifdef SGX
            ++it1;
#endif
        }

        clock_gettime(CLOCK_REALTIME, &tv);
        currTime = tv.tv_sec;
        if(currTime-beginTime>param->windowSize) {
            bool flag = true;
            int parallel = param->next_stage;
            while(flag) {
#ifdef NATIVE
                window_func(&parallel, output);
#else
                window_func(global_eid, &parallel, output);
#endif
                //std::cout << "Total message: " << output->total_msgs << std::endl;
                for (int k = 0; k < output->total_msgs; k++) {
#ifdef SGX
                    s_buff.add_msg(output->stream[k],output->routes[k][0], std::string(output->message[k], output->msg_len[k]),std::string((char*) output->mac[k], GCM_TAG_LEN), tv.tv_sec, tv.tv_nsec);
#else
                    s_buff.add_msg(output->stream[k], output->routes[k][0], std::string(output->message[k], output->msg_len[k]), tv.tv_sec, tv.tv_nsec);
#endif
                    s_buff.check_and_send(true);
                }

                if(output->total_msgs==0) {
                    flag = false;
                }

            }
	    struct timespec t;
            clock_gettime(CLOCK_REALTIME, &t);
                            long latency = calLatency(t.tv_sec, t.tv_nsec, tv.tv_sec, tv.tv_nsec);
            std::cout << "Window Latency: " << latency<<std::endl;
            beginTime = currTime;

        }
    }
    return NULL;
}

#ifdef NATIVE
void* Sink(void *arg, void (*enclave_func) (InputData*), void (*window_func)(OutputData*))
#else
void* Sink(void *arg,sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData*), sgx_status_t (*window_func)(sgx_enclave_id_t, OutputData*))
#endif
{
    Arguments * param = (Arguments*) arg;
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    unsigned long beginTime = tv.tv_sec;
    unsigned long currTime = tv.tv_sec;

    zmq::context_t * context;
    zmq::socket_t * sender, * receiver;
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int n = 0, m=0;
    zmq_init(arg, context, socks, &n, &m);
    std::cout << "Starting the count worker " << std::endl;
    //  Process tasks forever
    while(1) {
        zmq::message_t message;
        std::string topic = s_recv(*socks->receiver);
        socks->receiver->recv(&message);

        Message msg;
        msgpack::unpacked unpacked_body;
        msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char *> (message.data()), message.size());

        msgpack::object obj = oh.get();
        obj.convert(msg);
        std::vector<long> timeNSec = msg.timeNSec;
        std::vector<long> timeSec = msg.timeSec;

        OutputData * output = (OutputData* ) malloc(sizeof(OutputData));
        std::vector<std::string> msg_buffer = msg.value;
        char ct[100];
#ifdef SGX
        std::vector<std::string> mac_buffer = msg.gcm_tag;
        char tag[GCM_TAG_LEN];
#endif

        std::vector<std::string>::iterator it, it1;
        std::vector<long>::iterator it2, it3;
#ifdef SGX
        it1 = mac_buffer.begin();
#endif
        InputData * input = (InputData* ) malloc(sizeof(InputData));
//        OutputData * output = (OutputData* ) malloc(sizeof(OutputData));

        for(it = msg_buffer.begin(), it2=timeSec.begin(), it3=timeNSec.begin(); it != msg_buffer.end(); ++it, ++it2, ++it3) {
            clock_gettime(CLOCK_REALTIME, &tv);
            long latency = calLatency(tv.tv_sec, tv.tv_nsec, *it2, *it3);
            std::cout << "Network Latency: " << latency<<std::endl;

            std::string m = *it;
            input->msg_len = m.length();
            std::copy(m.begin(), m.end(), input->message);
#ifdef SGX
            std::string t = *it1;
            std::copy(t.begin(), t.end(), input->mac);
#endif
#ifdef NATIVE
            enclave_func(input);
#else
            enclave_func(global_eid, input);
#endif
//            enclave_count_execute(global_eid, ct , &ctLength, tag);

            struct timespec tv;
            clock_gettime(CLOCK_REALTIME, &tv);

            latency = calLatency(tv.tv_sec, tv.tv_nsec, *it2, *it3);
            std::cout << "Processing Latency: " << latency<<std::endl;
#ifdef SGX
            ++it1;
#endif
        }

        // If we have windows
        clock_gettime(CLOCK_REALTIME, &tv);
        currTime = tv.tv_sec;

        if(currTime-beginTime>param->windowSize) {
            std::cout << "Printing the map" << std::endl;
#ifdef NATIVE
            window_func(output);
#else
            window_func(global_eid, output);
#endif
            beginTime = currTime;

        }
    }
    return NULL;
}
