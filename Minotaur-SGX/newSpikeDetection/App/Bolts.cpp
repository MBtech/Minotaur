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
#include "Minotaur.hpp"


int func_main(int argc, char** argv) {
    std::map<std::string, int> parallelism;

    parallelism["spout"]  = 2;
    parallelism["ma"]  = 4;
    parallelism["spike"]  = 2;
    if(ROUTE_ALGO==4)
        parallelism["aggregate"] = 2;

    std::map<std::string, std::map<std::string, std::vector<std::string>>> topo, grouping;
    topo["spout"]["children"].push_back("ma");
    topo["ma"]["parents"].push_back("spout");
    if(ROUTE_ALGO==4) {
        topo["aggregate"]["children"].push_back("spike");
        topo["aggregate"]["parents"].push_back("ma");
        topo["spike"]["parents"].push_back("aggregate");
        topo["ma"]["children"].push_back("aggregate");
    } else {
        topo["spike"]["parents"].push_back("ma");
        topo["ma"]["children"].push_back("spike");
    }

    grouping["spout"]["out"].push_back("key");
    grouping["ma"]["in"].push_back("key");
    grouping["spike"]["in"].push_back("key");
    if(ROUTE_ALGO==4){
         grouping["ma"]["out"].push_back("key");
	grouping["aggregate"]["in"].push_back("key");
	grouping["aggregate"]["out"].push_back("key");
    }else{
        grouping["ma"]["out"].push_back("shuffle");
	}

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
            senderPort.push_back(atoi(argv[6]));
            senderIP.push_back(std::string(argv[5]));
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

    std::string file = "sensor_full.dat";

    if(strcmp(argv[1], "spout")==0) {
        Spout((void*) arg, file, enclave_spout_execute);
    } else if (strcmp(argv[1], "ma")==0) {
        arg->windowSize = 0;
        Bolt((void*) arg, enclave_ma_execute, dummy_window_func);
    } else if(strcmp(argv[1], "spike")==0) {
        arg->windowSize=0;
        Sink((void*) arg, enclave_spike_execute, dummy_window_sink);
    }else{
	arg->windowSize=0;
	Bolt((void*) arg, enclave_aggregate_execute, dummy_window_func);
	}

    return 0;
}

