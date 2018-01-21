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
    std::ifstream i("../healthcare.json");
    json j;
    i >> j;

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
    Arguments * arg = new Arguments;
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
    std::string file = "heart.csv";
    if(strcmp(argv[1], "spout")==0) {
        Spout((void*) arg,file, enclave_spout_execute);
    } else if (strcmp(argv[1], "vf")==0) {
        arg->windowSize = 0;
        Bolt((void*) arg, enclave_vf_execute, dummy_window_func);
    } else if(strcmp(argv[1], "af")==0) {
        arg->windowSize=0;
        Bolt((void*) arg, enclave_af_execute, dummy_window_func);
    } 
    else if(strcmp(argv[1], "emerg")==0) {
        arg->windowSize=0;
        Sink((void*) arg, enclave_emerg_execute, dummy_window_sink);
    }
    else if(strcmp(argv[1], "emerg1")==0) {
        arg->windowSize=0;
        Sink((void*) arg, enclave_emerg_execute, dummy_window_sink);
    }

    else if(strcmp(argv[1], "other")==0) {
        arg->windowSize=0;
        Sink((void*) arg, enclave_other_execute, dummy_window_sink);
    }
    /*
    else {
        arg->windowSize=10;
        Bolt((void*) arg, enclave_aggregate_execute, aggregate_window);
    }*/

    return 0;

}

