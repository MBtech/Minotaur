#include "../zhelpers.hpp"
//#include "zmq.hpp"
#include <string>
#include <functional>
#include "../utils.hpp"
#include "deciderExec.hpp"
//TODO: Need to do something with the zmq::socket

//void osend(const char * rmessage, void* sender) {
//    zmq::socket_t s = (zmq::socket_t) sender;
//    zmq::message_t message;
//    int len = sizeof (rmessage);
//    message.rebuild(len);
//    //std::cout << rmessage << std::endl;
//    sprintf((char *) message.data(), "%s", rmessage);
//    s.send(message);
//
//}

void split_vectors_d(std::vector<std::string> *v1, std::vector<std::string> *v2){
    v2->push_back(v1->back());
    v1->pop_back();
}

void split_vectors_d(std::vector<int> *v1, std::vector<int> *v2){
    v2->push_back(v1->back());
    v1->pop_back();
}

void* iot_decider(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort, 
        std::vector<std::string> receiverIP, std::vector<int> receiverPort) {
    struct Arguments * param = (Arguments*) arg;
    zmq::context_t context(1);
    //zmq::context_t context = zmq_ctx_new();
    
    std::vector<std::string> sender_emer_ip;
    split_vectors_d(&senderIP, &sender_emer_ip);
    

    std::vector<int> sender_emer_port;
    split_vectors_d(&senderPort, &sender_emer_port);

    zmq::socket_t sender = key_sender_conn(param, context, senderIP, senderPort);
    zmq::socket_t sender_emer = key_sender_conn(param, context, sender_emer_ip, sender_emer_port);

    std::cout << "Decider: Received the sender socket " << std::endl;

    zmq::socket_t receiver = key_receiver_conn(param, context, receiverIP.front(), receiverPort.front());

    std::cout << "Starting the decider worker " << std::endl;

    std::cout << "Reading messages, decider" << std::endl;
    //  Process tasks forever
    while (1) {

        zmq::message_t message;
        zmq::message_t message_emer;

        std::string word;
        receiver.recv(&message);
        std::string smessage(static_cast<char*> (message.data()), message.size());
        
        //std::cout << smessage << std::endl;
        int n = param->next_stage;

        StringArray * retmessage = NULL;
        int * retlen_a = NULL;
        int retlen = 0;
        int nextnode = 0;
        decider_enclave_execute(smessage, n, &sender, &retmessage, &retlen_a, &retlen, &nextnode);
        for (int k = 0; k < retlen; k++) {

            message.rebuild(retlen_a[k]);
            message_emer.rebuild(retlen_a[k]);

            //std::cout << retlen_a[k] << std::endl;
            snprintf((char *) message.data(), retlen_a[k], "%s", retmessage->array[k]);
            snprintf((char *) message_emer.data(), retlen_a[k], "%s", retmessage->array[k]);
            if(nextnode==1){
                sender_emer.send(message_emer);
            }
            sender.send(message);
            std::cout << retmessage->array[k] << std::endl;
        }
    }
    return NULL;
}
