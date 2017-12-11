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

#include "Bolts.hpp"

#ifdef SGX
void encrypt(char * line, size_t len_pt, unsigned char * gcm_ct, unsigned char * gcm_tag) {
    unsigned char * gcm_pt = reinterpret_cast<unsigned char *>(line);
    aes_gcm_encrypt(gcm_pt, len_pt, gcm_ct, gcm_tag);
}
#endif

void* spout (void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort)
{
    zmq::context_t *context = new zmq::context_t(1);
    struct Arguments * param = (Arguments*) arg;
//    std::cout << param->next_stage << std::endl;
    zmq::socket_t* sender = shuffle_sender_conn(param, *context, senderIP[0], senderPort[0]);
    std::cout << "Starting spout with id " << param->id << std::endl;
//    zmq::message_t* message = new zmq::message_t(1);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    std::ifstream datafile("book");
    //std::string ptsentence ("Hello is it me you are looking for?");
    std::string ptsentence;
    int j = 0;
    int n = param->next_stage;

    TimedBuffer s_buff(context,sender, BUFFER_TIMEOUT);
    sleep(10);
    while(1) {
        while(std::getline(datafile, ptsentence)) {
            boost::trim(ptsentence);
            //ptsentence = "Hark. They are speaking";
	    #ifdef SGX
            unsigned char gcm_ct [ptsentence.length()];
            uint8_t gcm_tag [16];

            encrypt(strdup(ptsentence.c_str()), ptsentence.length(), gcm_ct, gcm_tag);
            std::string ctsentence((char *)gcm_ct, (int)ptsentence.length());
            std::string mac((char*) gcm_tag, 16);
            #endif

	    Routes * routes = (Routes* ) malloc(sizeof(Routes));
            #ifdef NATIVE
            enclave_spout_execute((char*) ptsentence.c_str(),&n,&j, routes); 
            #else
            enclave_spout_execute(global_eid,(char*) ptsentence.c_str(),&n,&j, routes); 
            #endif
            struct timespec tv;
            clock_gettime(CLOCK_REALTIME, &tv);
	    for(int i=0; i<ROUTES; i++){
		std::cout << routes->array[0][i] << std::endl;
                std::cout << ptsentence << "  "<<  ptsentence.length()<<std::endl;
                #ifdef SGX
                s_buff.add_msg(routes->array[0][i], ctsentence, mac, tv.tv_sec, tv.tv_nsec);
		#else
		s_buff.add_msg(routes->array[0][i], ptsentence, tv.tv_sec, tv.tv_nsec);
		#endif
                s_buff.check_and_send();
	    }
            usleep(10);
        }
        datafile.clear();
        datafile.seekg(0);
    }
    return NULL;
}

void* splitter(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
               std::vector<std::string> receiverIP, std::vector<int> receiverPort) {
    struct Arguments * param = (Arguments*) arg;
    zmq::context_t* context = new zmq::context_t(1);
    //zmq::context_t context = zmq_ctx_new();
    zmq::socket_t* sender = key_sender_conn(param, *context, senderIP, senderPort);
    std::cout << "Splitter: Received the sender socket " << std::endl;
    zmq::socket_t receiver = shuffle_receiver_conn(param, *context, receiverIP, receiverPort);
    std::cout << "Starting the splitter worker with id " << param->id << std::endl;
    std::cout << "Reading messages, splitter" << std::endl;

    TimedBuffer s_buff(context,sender, BUFFER_TIMEOUT);
    //  Process tasks forever
    while (1) {
        zmq::message_t message;

        std::string word;
        std::string topic = s_recv(receiver);
        receiver.recv(&message);

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
	input->next_parallel = param->next_stage;
        #ifdef SGX
	it1 = mac_buffer.begin();
	#endif
        for(it = msg_buffer.begin(), it2=timeSec.begin(), it3=timeNSec.begin(); it != msg_buffer.end(); ++it, ++it2, ++it3) {
            std::string val = *it;
	    input->msg_len = val.length();
	    std::copy(val.begin(), val.end(), input->message);
	    #ifdef SGX
            std::string tag = *it1;
	    std::copy(tag.begin(), tag.end(), input->mac);
	    #endif
   	    #ifdef NATIVE
            enclave_splitter_execute(input, output);
		#else
            enclave_splitter_execute(global_eid, input, output);
		#endif
	    //std::cout << "Total message: " << output->total_msgs << std::endl;
            for (int k = 0; k < output->total_msgs; k++) {
		#ifdef SGX
                s_buff.add_msg(output->routes[k][0], std::string(output->message[k], output->msg_len[k]),std::string((char*) output->mac[k], 16), *it2, *it3);
		#else
                s_buff.add_msg(output->routes[k][0], std::string(output->message[k], output->msg_len[k]), *it2, *it3);
		#endif
                s_buff.check_and_send();
            }
	    #ifdef SGX
            ++it1;
	    #endif
        }

    }
    return NULL;
}

void* count(void *arg, std::vector<std::string> receiverIP, std::vector<int> receiverPort)
{
    struct Arguments * param = (Arguments*) arg;
    zmq::context_t context(1);
    zmq::socket_t receiver = key_receiver_conn(param, context, receiverIP[0], receiverPort[0]);

    std::cout << "Starting the count worker " << std::endl;
    //  Process tasks forever
    while(1) {
        zmq::message_t message;
        std::string topic = s_recv(receiver);
        receiver.recv(&message);

        Message msg;
        msgpack::unpacked unpacked_body;
        msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char *> (message.data()), message.size());

        msgpack::object obj = oh.get();
        obj.convert(msg);
        std::vector<long> timeNSec = msg.timeNSec;
        std::vector<long> timeSec = msg.timeSec;

        std::vector<std::string> msg_buffer = msg.value;
        char ct[100];
	#ifdef SGX
        std::vector<std::string> mac_buffer = msg.gcm_tag;
        char tag[16];
	#endif

        std::vector<std::string>::iterator it, it1;
        std::vector<long>::iterator it2, it3;
	#ifdef SGX
	it1 = mac_buffer.begin();
	#endif
        InputData * input = (InputData* ) malloc(sizeof(InputData));
//        OutputData * output = (OutputData* ) malloc(sizeof(OutputData));

        for(it = msg_buffer.begin(), it2=timeSec.begin(), it3=timeNSec.begin(); it != msg_buffer.end(); ++it, ++it2, ++it3) { 
            std::string m = *it;
            input->msg_len = m.length();
            std::copy(m.begin(), m.end(), input->message);
	    #ifdef SGX
            std::string t = *it1;
            std::copy(t.begin(), t.end(), input->mac);
	    #endif
   	#ifdef NATIVE
	    enclave_count_execute(input);
	#else
	    enclave_count_execute(global_eid, input);
	#endif
//            enclave_count_execute(global_eid, ct , &ctLength, tag);

            struct timespec tv;
            clock_gettime(CLOCK_REALTIME, &tv);
	    
            long latency = calLatency(tv.tv_sec, tv.tv_nsec, *it2, *it3);
            std::cout << "Latency: " << latency<<std::endl;
	    #ifdef SGX
	    ++it1;
	    #endif
        }
    }
    return NULL;
}


int func_main(int argc, char** argv) {
    const int count_threads = 6;
    const int split_threads = 4;
    const int spout_threads = 2;

    pthread_t spout_t[spout_threads];
    pthread_t split_t[split_threads];
    pthread_t count_t[count_threads];

    if(strcmp(argv[1], "spout")==0) {
        std::cout << spout_threads << std::endl;
        std::cout << "Starting spout" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = split_threads;
        arg->prev_stage = 0;
        std::vector<int> senderPort;
        std::vector<std::string> senderIP;
        senderPort.push_back(atoi(argv[4]));
        senderIP.push_back(std::string(argv[3]));
        spout((void*) arg, senderIP, senderPort);
    }
    if(strcmp(argv[1], "splitter")==0) {
        std::cout << "Starting splitter" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]) ;
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;

        std::ifstream senderfile("countIP");
        std::string ip, port;
        while(senderfile >> ip>>port) {
            senderIP.push_back(ip);
            senderPort.push_back(stoi(port));
            std::cout << ip << " " << port << std::endl;
        }

        std::ifstream receiverfile("spoutIP");
        ip, port;
        while(receiverfile >> ip>>port) {
            receiverIP.push_back(ip);
            receiverPort.push_back(stoi(port));
        }
        splitter((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "count")==0) {
        std::cout << "Starting count" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]) ;
        arg->next_stage = 0;
        arg->prev_stage = split_threads;
        std::vector<int> receiverPort;
        std::vector<std::string> receiverIP;
        receiverPort.push_back(atoi(argv[4]));
        receiverIP.push_back(std::string(argv[3]));
        count((void*)arg, receiverIP, receiverPort);
    }

    return 0;
}

