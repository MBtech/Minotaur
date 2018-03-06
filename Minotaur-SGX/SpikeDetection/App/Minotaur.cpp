#include "Minotaur.hpp"
#include <thread>

#ifdef SGX
void encrypt(char * line, size_t len_pt, unsigned char * gcm_ct, unsigned char * gcm_tag) {
    unsigned char * gcm_pt = reinterpret_cast<unsigned char *>(line);
    aes_gcm_encrypt(gcm_pt, len_pt, gcm_ct, gcm_tag);
}
#endif

void dumbVals(int* counter, std::string name, int id) {
    std::ofstream out(name+"processedlog"+std::to_string(id));

    while(1) {
        struct timespec tv;
        clock_gettime(CLOCK_REALTIME, &tv);
        out << "Tuples processed: " << *counter << ",Time: "<< tv.tv_sec << "." <<tv.tv_nsec << "\n";
//        std::cout << "Tuples processed: " << *counter << std::endl;
        usleep(10000);
    }
    out.close();
}

void boltWindow(TimedBuffer* s_buff, Arguments *param, OutputData * output, unsigned long oldestTime, unsigned long oldestTimeN, bool * newWindow,
                sgx_status_t (*window_func)(sgx_enclave_id_t, Parallelism* n, OutputData*)) {
    while(true) {
        struct timespec tv;
        usleep(param->windowSize*1000000);
        clock_gettime(CLOCK_REALTIME, &tv);
        bool flag = true;
        std::vector<int> para = param->next_stage;
        Parallelism * parallel = (Parallelism* ) malloc(sizeof(Parallelism));
        std::copy(para.begin(), para.end(), parallel->next_parallel);
        while(flag) {
#ifdef NATIVE
            window_func(parallel, output);
#else
            window_func(global_eid, parallel, output);
#endif
            for (int k = 0; k < output->total_msgs; k++) {
#ifdef SGX
                s_buff->add_msg(output->stream[k],output->routes[k], std::string(output->message[k], output->msg_len[k]),std::string((char*) output->mac[k], GCM_TAG_LEN), tv.tv_sec, tv.tv_nsec);
#else
                s_buff->add_msg(output->stream[k], output->routes[k], std::string(output->message[k], output->msg_len[k]), tv.tv_sec, tv.tv_nsec);
#endif
                s_buff->check_and_send(true);
            }

            if(output->total_msgs==0) {
                flag = false;
            }

        }

        struct timespec t;
        clock_gettime(CLOCK_REALTIME, &t);
        long latency = calLatency(t.tv_sec, t.tv_nsec, oldestTime, oldestTimeN);
        std::cout << "L:" << latency<<std::endl;
        *newWindow = true;
    }
}

#ifdef NATIVE
void* Spout (void *arg, std::string file, void (*enclave_func) (InputData* , OutputSpout*))
#else
void* Spout (void *arg, std::string file,  sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData* , OutputSpout*))
#endif
{
    zmq::context_t * context;
    zmq::socket_t * sender, * receiver;
    struct Arguments * param = (Arguments*) arg;
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int  m=0;
    std::vector<int> n;
    zmq_init(arg, context, socks, &n, &m);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    std::ifstream datafile(file);
    //std::string ptsentence ("Hello is it me you are looking for?");
    std::string ptsentence;
    std::vector<zmq::socket_t*> sockets(socks->sender, socks->sender+(param->out_grouping.size()*sizeof(zmq::socket_t*)));
    TimedBuffer s_buff(context,sockets, BUFFER_TIMEOUT);
    std::vector<std::string> datavector;
    while(std::getline(datafile, ptsentence)) {
        datavector.push_back(ptsentence);
    }
    sleep(5);

    int counter = 0;
    // Start measurement thread
    std::thread t1(dumbVals, &counter, param->name, param->id);
    while(1) {
        for(int in = 0; in<datavector.size(); in++) {
            struct timespec tv;
            clock_gettime(CLOCK_REALTIME, &tv);
            ptsentence = datavector[in];
            boost::trim(ptsentence);
            //ptsentence = "Hark. They are speaking";
#ifdef SGX
            unsigned char gcm_ct [ptsentence.length()];
            uint8_t gcm_tag [GCM_TAG_LEN];

            encrypt(strdup(ptsentence.c_str()), ptsentence.length(), gcm_ct, gcm_tag);
            std::string ctsentence((char *)gcm_ct, (int)ptsentence.length());
            std::string mac((char*) gcm_tag, GCM_TAG_LEN);
#endif
            InputData * input = (InputData* ) malloc(sizeof(InputData));
            OutputSpout * output = (OutputSpout* ) malloc(sizeof(OutputSpout));
            std::copy(n.begin(), n.end(), input->next_parallel);

            std::copy(ctsentence.begin(), ctsentence.end(), input->message);
            std::copy(mac.begin(), mac.end(), input->mac);
            input->msg_len = ctsentence.length();
            input->source = param->id;
#ifdef NATIVE
            enclave_func(input, output);
#else
            enclave_func(global_eid, input, output);
#endif
            int R = 0;
            if(strcmp(param->out_grouping[0].c_str(), "shuffle")==0) {
                R = 1;
            }
            else {
                R = ROUTES;
            }
            for(int i=0; i<R; i++) {
                //std::cout << routes->array[0][i] << std::endl;
//                std::cout << ptsentence << "  "<<  ptsentence.length()<<std::endl;
#ifdef SGX
                s_buff.add_msg(output->stream[i],output->routes[i], std::string(output->message[i], output->msg_len[i]), std::string((char*) output->mac[i], GCM_TAG_LEN), tv.tv_sec, tv.tv_nsec);
#else
                s_buff.add_msg(output->stream[i], output->routes[], ptsentence, tv.tv_sec, tv.tv_nsec);
#endif
                s_buff.check_and_send(false);
            }
            usleep(SLEEP);
            counter++;
        }
        //datafile.clear();
        //datafile.seekg(0);
    }
    return NULL;
}

#ifdef NATIVE
void* Bolt(void *arg, void (*enclave_func) (InputData* , OutputData*), void(*window_func)(Parallelism *, OutputData*)) {
#else
void* Bolt(void *arg, sgx_status_t (*enclave_func) (sgx_enclave_id_t, InputData* , OutputData*),
           sgx_status_t (*window_func)(sgx_enclave_id_t, Parallelism* n, OutputData*) ) {
#endif
    Arguments * param = (Arguments*) arg;
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    //float beginTime = (float)tv.tv_sec + (float)tv.tv_nsec/1000000000.0f;
    struct timespec beginTime = tv;
//    float currTime = (float)tv.tv_sec + (float)tv.tv_nsec/1000000000.0f;

    zmq::context_t * context;
    zmq::socket_t * sender, * receiver;
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int m=0;
    std::vector<int> n;
    zmq_init(arg, context, socks, &n, &m);
    std::vector<zmq::socket_t*> sockets(socks->sender, socks->sender+(param->out_grouping.size()*sizeof(zmq::socket_t*)));
    TimedBuffer s_buff(context,sockets, BUFFER_TIMEOUT);
    bool newWindow = true;
    unsigned long oldestTime, oldestTimeN;

    int counter = 0;
    // Start measurement thread
    std::thread t1(dumbVals, &counter, param->name, param->id);

    int R = 0;
    if(!param->multiout) {
        R = 1;
    }
    else {
        R = ROUTES;
    }
    InputData * input = (InputData* ) malloc(sizeof(InputData));
    OutputData * output = (OutputData* ) malloc(sizeof(OutputData));

//    if(param->windowSize>0.0) {
//        std::thread windowThread(boltWindow, &s_buff,param, output,oldestTime, oldestTimeN, &newWindow,  window_func);
//    }
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

//        InputData * input = (InputData* ) malloc(sizeof(InputData));
//        OutputData * output = (OutputData* ) malloc(sizeof(OutputData));
        std::vector<std::string> msg_buffer = msg.value;
#ifdef SGX
        std::vector<std::string> mac_buffer = msg.gcm_tag;
#endif

        std::vector<std::string>::iterator it, it1;
        std::vector<long>::iterator it2, it3;

        std::copy(n.begin(), n.end(), input->next_parallel);
        int index = 0;
#ifdef SGX
        it1 = mac_buffer.begin();
#endif
        for(it = msg_buffer.begin(), it2=timeSec.begin(), it3=timeNSec.begin(); index<msg_buffer.size(); ++it, ++it2, ++it3) {
            counter++;
            if(newWindow) {
                oldestTime = *it2;
                oldestTimeN = *it3;
                newWindow = false;
            }
//            clock_gettime(CLOCK_REALTIME, &tv);
//            long latency = calLatency(tv.tv_sec, tv.tv_nsec, *it2, *it3);
//            std::cout << "Network Latency: " << latency<<std::endl;

            std::string val = *it;
            input->msg_len = val.length();
            input->source = param->id;
            std::copy(val.begin(), val.end(), input->message);
#ifdef SGX
            std::string tag = mac_buffer[index];
            std::copy(tag.begin(), tag.end(), input->mac);
#endif
#ifdef NATIVE
            enclave_func(input, output);
#else
            enclave_func(global_eid, input, output);
#endif
            if(param->windowSize==0) {
//                struct timespec t;
//                clock_gettime(CLOCK_REALTIME, &t);
//                long latency = calLatency(t.tv_sec, t.tv_nsec, tv.tv_sec, tv.tv_nsec);
//                std::cout << "Processing Latency: " << latency<<std::endl;
                //std::cout << "Total message: " << output->total_msgs << std::endl;
                for (int k = 0; k < output->total_msgs; k++) {

#ifdef SGX
//                    s_buff.add_msg(output->stream[k],output->routes[k][0], std::string(output->message[k], output->msg_len[k]),std::string((char*) output->mac[k], GCM_TAG_LEN), tv.tv_sec, tv.tv_nsec);
                    s_buff.add_msg(output->stream[k],output->routes[k], std::string(output->message[k], output->msg_len[k]),std::string((char*) output->mac[k], GCM_TAG_LEN), *it2, *it3);
#else
                    s_buff.add_msg(output->stream[k], output->routes[k], std::string(output->message[k], output->msg_len[k]), *it2, *it3);
                    //s_buff.add_msg(output->stream[k], output->routes[k][0], std::string(output->message[k], output->msg_len[k]), tv.tv_sec, tv.tv_nsec);
#endif
                    s_buff.check_and_send(false);
#ifdef SGX
                    ++it1;
#endif
                }
            }
            index ++;
        }

        if(param->windowSize >0.0) {
            clock_gettime(CLOCK_REALTIME, &tv);
            struct timespec currTime = tv;
            //currTime = (float)tv.tv_sec + (float)tv.tv_nsec/1000000000.0f;
            //std::cout << tv.tv_nsec << " " << (float)tv.tv_nsec/1000000000.0f << std::endl;
            //printf("%.2f, %.2f\n", currTime, beginTime);
            long diff = calLatency(currTime.tv_sec, currTime.tv_nsec, beginTime.tv_sec, beginTime.tv_nsec);
            if(diff>=param->windowSize*1000000) {
                //std::cout << currTime - beginTime << std::endl;
                bool flag = true;
                std::vector<int> para = param->next_stage;
                Parallelism * parallel = (Parallelism* ) malloc(sizeof(Parallelism));
                std::copy(para.begin(), para.end(), parallel->next_parallel);
                while(flag) {
#ifdef NATIVE
                    window_func(parallel, output);
#else
                    window_func(global_eid, parallel, output);
#endif
                    for (int k = 0; k < output->total_msgs; k++) {
#ifdef SGX
                        s_buff.add_msg(output->stream[k],output->routes[k], std::string(output->message[k], output->msg_len[k]),std::string((char*) output->mac[k], GCM_TAG_LEN), tv.tv_sec, tv.tv_nsec);
#else
                        s_buff.add_msg(output->stream[k], output->routes[k], std::string(output->message[k], output->msg_len[k]), tv.tv_sec, tv.tv_nsec);
#endif
                        s_buff.check_and_send(true);
                    }

                    if(output->total_msgs==0) {
                        flag = false;
                    }

                }

                struct timespec t;
                clock_gettime(CLOCK_REALTIME, &t);
                long latency = calLatency(t.tv_sec, t.tv_nsec, oldestTime, oldestTimeN);
                std::cout << "L:" << latency<<std::endl;
                beginTime = currTime;
                newWindow = true;


            }
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
    struct timespec beginTime = tv;

    zmq::context_t * context;
    zmq::socket_t * sender, * receiver;
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int m=0;
    std::vector<int> n;
    zmq_init(arg, context, socks, &n, &m);
    std::cout << "Starting the count worker " << std::endl;

    int counter = 0;
    // Start measurement thread
    std::thread t1(dumbVals, &counter, param->name, param->id);

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
            counter++;
            clock_gettime(CLOCK_REALTIME, &tv);
            //long latency = calLatency(tv.tv_sec, tv.tv_nsec, *it2, *it3);
            //std::cout << "Network Latency: " << latency<<std::endl;

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

            //struct timespec tv;
            clock_gettime(CLOCK_REALTIME, &tv);

            long latency = calLatency(tv.tv_sec, tv.tv_nsec, *it2, *it3);
            //std::cout << "Processing Latency: " << latency<<std::endl;
            std::cout << "L:" << latency<<std::endl;
#ifdef SGX
            ++it1;
#endif
        }

        // If we have windows
        clock_gettime(CLOCK_REALTIME, &tv);
	long diff = calLatency(tv.tv_sec, tv.tv_nsec, beginTime.tv_sec, beginTime.tv_nsec);
//        std::cout << currTime - beginTime << std::endl;
        if(diff>=param->windowSize * 1000000) {
            std::cout << "Printing the map" << std::endl;
#ifdef NATIVE
            window_func(output);
#else
            window_func(global_eid, output);
#endif
            beginTime = tv;

        }
    }
    return NULL;
}
