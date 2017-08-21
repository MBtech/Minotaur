#include "../zmq.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "../utils.hpp"
#include "i_spoutExec.hpp"
#include <fstream>
#include <streambuf>
#include <sstream>

std::string prepareMessage(int hamOrSpam, int trainingOrAnalysis) {
    std::string spoutMessage = "";
    return spoutMessage;

}

void* iot_spout (void *arg, std::string ip, int port)
{
    zmq::context_t context (1);
    struct Arguments * param = (Arguments*) arg;
    zmq::socket_t sender = shuffle_sender_conn(param, context, ip, port);
    std::cout << "Sending tasks to workers…\n" << std::endl;
    zmq::message_t message(2);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    std::string spoutMessage ("1 2 12/12/2012 temperature 38");
    
    int n = param->next_stage;
    int j = 0;
    std::cout << "before while" << std::endl;

    while(1) {
        int count =0;
        while(count < 100000000) {
            count++;
        }
        i_enclave_execute(&j,&n);

        int len = spoutMessage.length()+std::to_string(j).length() + 2;
        message.rebuild(len);

        char * cstr = new char [spoutMessage.length()+1];
        std::strcpy (cstr, spoutMessage.c_str());

        sprintf((char *)message.data(),"%d %s",j,spoutMessage.c_str());
        std::cout << spoutMessage << std::endl;
        sender.send(message);
    }
    return NULL;
}
