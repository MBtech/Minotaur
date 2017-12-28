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

void* spout (void *arg)
{
    zmq::context_t * context;
    zmq::socket_t * sender, * receiver; 
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int n = 0, m=0;
    zmq_init(arg, context, socks, &n, &m);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    std::ifstream datafile("book");
    //std::string ptsentence ("Hello is it me you are looking for?");
    std::string ptsentence;
    int j = 0;

    TimedBuffer s_buff(context,socks->sender, BUFFER_TIMEOUT);
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
            for(int i=0; i<ROUTES; i++) {
                std::cout << routes->array[0][i] << std::endl;
                std::cout << ptsentence << "  "<<  ptsentence.length()<<std::endl;
#ifdef SGX
                s_buff.add_msg(routes->array[0][i], ctsentence, mac, tv.tv_sec, tv.tv_nsec);
#else
                s_buff.add_msg(routes->array[0][i], ptsentence, tv.tv_sec, tv.tv_nsec);
#endif
                s_buff.check_and_send();
            }
            usleep(5);
        }
        datafile.clear();
        datafile.seekg(0);
    }
    return NULL;
}

void* splitter(void *arg) {
    zmq::context_t * context;
    zmq::socket_t * sender, * receiver;
    Sockets* socks = (Sockets*) malloc(sizeof(Sockets));
    int n = 0, m=0;
    zmq_init(arg, context, socks, &n, &m);
    TimedBuffer s_buff(context,socks->sender, BUFFER_TIMEOUT);
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

void* count(void *arg)
{
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
    std::map<std::string, int> parallelism;

    parallelism["spout"]  = 2;
    parallelism["splitter"]  = 4;
    parallelism["count"]  = 6;

    std::map<std::string, std::map<std::string, std::vector<std::string>>> topo, grouping;
    topo["spout"]["children"].push_back("splitter");
    topo["splitter"]["parents"].push_back("spout");
    topo["splitter"]["children"].push_back("count");
    topo["count"]["parents"].push_back("splitter");

    grouping["spout"]["out"].push_back("shuffle");
    grouping["splitter"]["in"].push_back("shuffle");
    grouping["splitter"]["out"].push_back("key");
    grouping["count"]["in"].push_back("key");

// TODO: Adapt this code to work for multiple output stages
    Arguments * arg = new Arguments;
    arg->id = atoi(argv[2]);

    if(topo[argv[1]]["parents"].size()>0 ) {
        arg->prev_stage = parallelism[topo[argv[1]]["parents"].back()];
        arg->in_grouping.push_back(grouping[argv[1]]["in"].back());
    } else {
        arg->prev_stage = 0;
    }
    if(topo[argv[1]]["children"].size()>0 ) {
        arg->next_stage = parallelism[topo[argv[1]]["children"].back()];
        arg->out_grouping.push_back(grouping[argv[1]]["out"].back());
    } else {
        arg->next_stage = 0;
    }
    std::vector<std::string> senderIP, receiverIP;
    std::vector<int> senderPort, receiverPort;
    if(grouping[argv[1]]["in"].size()>0) {
	if(grouping[argv[1]]["in"].back() =="key" ) {
            receiverPort.push_back(atoi(argv[4]));
            receiverIP.push_back(std::string(argv[3]));
        } else {
            std::string filename = topo[argv[1]]["parents"].back() + "IP";
            std::ifstream receiverfile(filename);
            std::string ip, port;
            while(receiverfile >> ip>>port) {
                receiverIP.push_back(ip);
                receiverPort.push_back(stoi(port));
                std::cout << ip << " " << port << std::endl;
            }
        }
    }
    if(grouping[argv[1]]["out"].size()>0) {
        if(grouping[argv[1]]["out"].back() == "shuffle" ) {
            senderPort.push_back(atoi(argv[4]));
            senderIP.push_back(std::string(argv[3]));
        } else {
            std::string filename = topo[argv[1]]["children"].back() + "IP";
            std::ifstream senderfile(filename);
            std::string ip, port;
            while(senderfile >> ip>>port) {
                senderIP.push_back(ip);
                senderPort.push_back(stoi(port));
                std::cout << ip << " " << port << std::endl;
            }
        }
    }

    arg -> senderIP = senderIP;
    arg -> receiverIP = receiverIP;
    arg -> senderPort = senderPort;
    arg-> receiverPort = receiverPort;

    if(strcmp(argv[1], "spout")==0) {
        spout((void*) arg);
    } else if (strcmp(argv[1], "splitter")==0) {
        splitter((void*) arg);
    } else if(strcmp(argv[1], "count")==0) {
        count((void*) arg);
    }

    return 0;
}

