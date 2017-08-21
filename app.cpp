/* 
 * File:   app.cpp
 * Author: mb
 *
 * Created on May 16, 2017, 4:49 PM
 */

#include <cstdlib>
#include<iostream>
#include "Server.h"
#include "Client.h"
#include <pthread.h>
#include <unistd.h>
#include "utils.hpp"
#include <stdlib.h>


//using namespace std;

void* spout (void *arg, std::string ip, int port);
void* splitter(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort, 
        std::vector<std::string> receiverIP, std::vector<int> receiverPort);
void* count(void *arg, std::string receiverIP, int port);

void* iot_spout (void *arg, std::string ip, int port);
void* iot_tokenizer(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
        std::vector<std::string> receiverIP, std::vector<int> receiverPort);
void* iot_decider(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
        std::vector<std::string> receiverIP, std::vector<int> receiverPort);
void* iot_emergency(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
        std::vector<std::string> receiverIP, std::vector<int> receiverPort);
void* iot_pattern(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
        std::vector<std::string> receiverIP, std::vector<int> receiverPort);
void* iot_storage(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
        std::vector<std::string> receiverIP, std::vector<int> receiverPort);
void* iot_notify(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort,
        std::vector<std::string> receiverIP, std::vector<int> receiverPort);

/* Right now it can only be used to start up different components of a word count program
 * To start up a spout use: ./app spout <spoutID> <spoutIP> <spoutPort>
 * To start up a split use: ./app splitter <splitID> <n> <countIP1> <countPort1> ... <countIPn> <countPortn>
 * <m> <spoutIP1> <spoutPort1> ... <spoutIPm> <spoutPortm>
 * To start up a count use: ./app count <countID> <countIP> <countPort>
 */
int main(int argc, char** argv) {
    
    const int count_threads = 1;
    const int split_threads = 1;
    const int spout_threads = 1;
    
    pthread_t spout_t[spout_threads];
    pthread_t split_t[split_threads];
    pthread_t count_t[count_threads];
    
    std::cout << argv[1] << std::endl;
    if(strcmp(argv[1], "spout")==0){
        std::cout << "Starting spout" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = split_threads;
        arg->prev_stage = 0;
        spout((void*) arg, argv[3], atoi(argv[4])); 
    }
    if(strcmp(argv[1], "iot_spout")==0){
        std::cout << "Starting iot_spout" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = split_threads;
        arg->prev_stage = 0;
        iot_spout((void*) arg, argv[3], atoi(argv[4]));
    }
    if(strcmp(argv[1], "splitter")==0){
        std::cout << "Starting splitter" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }
            
        splitter((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "iot_tokenizer")==0){
        std::cout << "Starting iot_tokenizer" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }

        iot_tokenizer((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "iot_decider")==0){
        std::cout << "Starting iot_decider" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }

        iot_decider((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "iot_emergency")==0){
        std::cout << "Starting iot_emergency" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }

        iot_emergency((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "iot_pattern")==0){
        std::cout << "Starting iot_pattern" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }

        iot_pattern((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "iot_storage")==0){
        std::cout << "Starting iot_storage" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }

        iot_storage((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }
    if(strcmp(argv[1], "iot_notify")==0){
        std::cout << "Starting iot_notify" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = count_threads;
        arg->prev_stage = spout_threads;
        std::vector<std::string> senderIP, receiverIP;
        std::vector<int> senderPort, receiverPort;
        int nSender = atoi(argv[3]);
        int nReceiver = atoi(argv[3+(nSender*2)+1]);
        for(int i=0; i<nSender; i++){
            senderIP.push_back(argv[4+i]);
            std::cout << senderIP[i] << std::endl;
            senderPort.push_back(atoi(argv[4+nSender+i]));
            std::cout << senderPort[i] << std::endl;
        }
        for(int i=0; i<nReceiver; i++){
            receiverIP.push_back(argv[4+(nSender*2)+i+1]);
            std::cout << receiverIP[i] << std::endl;
            receiverPort.push_back(atoi(argv[4+(2*nSender)+nReceiver+i+1]));
            std::cout << receiverPort[i] << std::endl;
        }

        iot_notify((void *)arg, senderIP, senderPort, receiverIP, receiverPort);
    }

    if(strcmp(argv[1], "count")==0){
        std::cout << "Starting count" << std::endl;
        Arguments *arg = new Arguments;
        arg->id = atoi(argv[2]);
        arg->next_stage = 0;
        arg->prev_stage = split_threads;
        count((void*)arg, argv[3], atoi(argv[4]));
    }
////    struct Arguments arg;
//    for (int j=0; j<spout_threads; j++){
//        Arguments *arg = new Arguments;
//        arg->id = j;
//        arg->next_stage = split_threads;
//        arg->prev_stage = 0;
//        pthread_create(&spout_t[j], NULL, spout, (void*)arg);  
//        sleep(1);
//        //delete arg;
//    }
//    for (int j=0; j<split_threads; j++){
//        Arguments *arg = new Arguments;
//        arg->id = j;
//        arg->next_stage = count_threads;
//        arg->prev_stage = spout_threads;
//        pthread_create(&split_t[j], NULL, splitter, (void*)arg);
//        sleep(1);
//        //delete arg;
//    }
//    for (int j=0; j<count_threads; j++){
//        Arguments *arg = new Arguments;
//        arg->id = j;
//        arg->next_stage = 0;
//        arg->prev_stage = split_threads;
//        pthread_create(&count_t[j], NULL, count, (void*)arg);
//        sleep(1);
//        //delete arg;
//    }
//    std::cout << "Started the components…" << std::endl;
//    while(true){
//        continue;
//    }
    return 0;
}

