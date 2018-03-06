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
#include "json.hpp"

using json = nlohmann::json;

int func_main(int argc, char** argv) {

    //std::map<std::string, int> parallelism;
    std::ifstream i(std::string("../")+std::string(argv[3]));
    json j;
    i >> j;
    Arguments * arg = new Arguments;
    std::cout << argv[1] << std::endl;
    if(strcmp(argv[1], "observer")!=0) {
        std::map<std::string, int> parallelism = j["parallelism"];
        std::map<std::string, std::map<std::string, std::vector<std::string>>> topo, grouping;
        std::map<std::string, std::vector<std::string>> topology= j["topology"];
        std::vector<std::string> components = j["components"];
        for(int i =0; i<components.size()-1; i++) {
            for(int j=0; j<topology[components[i]].size(); j=j+2) {
                grouping[components[i]]["out"].push_back(topology[components[i]][j+1]);
                grouping[topology[components[i]][j]]["in"].push_back(topology[components[i]][j+1]);
                topo[components[i]]["children"].push_back(topology[components[i]][j]);
                topo[topology[components[i]][j]]["parents"].push_back(components[i]);
            }
        }
// TODO: Adapt this code to work for multiple input stages i.e join like scenario
        arg->id = atoi(argv[2]);

        if(topo[argv[1]]["parents"].size()>0 ) {
            arg->prev_stage = parallelism[topo[argv[1]]["parents"].back()];
            arg->in_grouping.push_back(grouping[argv[1]]["in"].back());
        } else {
            arg->prev_stage = 0;
        }
        if(topo[argv[1]]["children"].size()>0 ) {
            for(int i =0; i<topo[argv[1]]["children"].size(); i++) {
                arg->next_stage.push_back(parallelism[topo[argv[1]]["children"][i]]);
                arg->out_grouping.push_back(grouping[argv[1]]["out"][i]);
            }
        }

        std::vector<std::vector<std::string>> receiverIP(1), senderIP(grouping[argv[1]]["out"].size());
        std::vector<std::vector<int>> receiverPort(1), senderPort(grouping[argv[1]]["out"].size());

        if(grouping[argv[1]]["in"].size()>0) {
            /*        if(grouping[argv[1]]["in"].back() =="key" ) {
                        receiverPort[0].push_back(atoi(argv[4]));
                        receiverIP[0].push_back(std::string(argv[3]));
                    } else {
            */
            std::string filename = std::string(argv[1])+ "_in_0" + "_IP";
            std::ifstream receiverfile(filename);
            std::string ip, port;
            while(receiverfile >> ip>>port) {
                receiverIP[0].push_back(ip);
                receiverPort[0].push_back(stoi(port));
                std::cout << ip << " " << port << std::endl;
            }
//        }
        }
        if(grouping[argv[1]]["out"].size()>0) {
            for(int i=0; i<grouping[argv[1]]["out"].size(); i++) {
                /*            if(grouping[argv[1]]["out"][i] == "shuffle" ) {
                                senderPort[i].push_back(atoi(argv[4+(2*i)]));
                                senderIP[i].push_back(std::string(argv[3+(2*i)]));
                            } else {
                */
                std::string filename = std::string(argv[1])+ "_out_" + std::to_string(i) + "_IP";
                std::ifstream senderfile(filename);
                std::string ip, port;
                while(senderfile >> ip>>port) {
                    senderIP[i].push_back(ip);
                    senderPort[i].push_back(stoi(port));
                    std::cout << ip << " " << port << std::endl;
                }
//            }
            }
        }
        arg -> senderIP = senderIP;
        arg -> receiverIP = receiverIP;
        arg -> senderPort = senderPort;
        arg-> receiverPort = receiverPort;
        arg->name = std::string(argv[1]);
        std::vector<std::string> files;
	files.push_back("zipf_data_1.2");
	files.push_back("zipf_data_1.6");
	files.push_back("zipf_data_1.4");
        if(strcmp(argv[1], "spout")==0) {
            Spout((void*) arg,files, enclave_spout_execute);
        } else if (strcmp(argv[1], "splitter")==0) {
            arg->windowSize = 0.0f;
            arg->multiout = true;
	    arg->feedbackIP.push_back(j["servers"][0]);
            arg->observerIP.push_back(j["servers"][0]);
            arg->observerPort.push_back(7000);
	    arg->feedbackPort.push_back(7001);
            Bolt((void*) arg, enclave_splitter_execute, dummy_window_func);
        } else if(strcmp(argv[1], "count")==0) {
            arg->windowSize=1.0f;
            Sink((void*) arg, enclave_count_execute, count_window);
        }
        else {
            arg->windowSize=0.05f;
            Bolt((void*) arg, enclave_aggregate_execute, aggregate_window);
        }
    } else if(strcmp(argv[1], "observer")==0) {
        std::vector<std::vector<std::string>> receiverIP(1), senderIP(1);
        std::vector<std::vector<int>> receiverPort(1), senderPort(1);
        arg->id = 0;
        arg->name = std::string(argv[1]);
        arg->prev_stage = j["parallelism"]["aggregate"];
        arg->in_grouping.push_back("key");
        arg->out_grouping.push_back("shuffle");
        senderIP[0].push_back(j["servers"][0]);
        senderPort[0].push_back(7001);
        receiverIP[0].push_back(j["servers"][0]);
        receiverPort[0].push_back(7000);
        arg->receiverIP = receiverIP;
        arg->receiverPort = receiverPort;
        arg->senderIP = senderIP;
        arg->senderPort = senderPort;
        arg->windowSize=0;
        Observer((void*) arg, enclave_observer_execute, dummy_sink_func);
    }
    return 0;

}

