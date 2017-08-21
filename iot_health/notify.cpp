#include "../zhelpers.hpp"
//#include "zmq.hpp"
#include <string>
#include <functional>
#include "../utils.hpp"
#include "notifyExec.hpp"
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

void* iot_notify(void *arg, std::vector<std::string> senderIP, std::vector<int> senderPort, 
        std::vector<std::string> receiverIP, std::vector<int> receiverPort) {
    struct Arguments * param = (Arguments*) arg;
    zmq::context_t context(1);
    //zmq::context_t context = zmq_ctx_new();
    zmq::socket_t sender = key_sender_conn(param, context, senderIP, senderPort);
    std::cout << "Notify: Received the sender socket " << std::endl;

    zmq::socket_t receiver = key_receiver_conn(param, context, receiverIP.front(), receiverPort.front());

    std::cout << "Starting the notify worker " << std::endl;

    std::cout << "Reading messages, notify" << std::endl;
    //  Process tasks forever
    while (1) {
        zmq::message_t message;

        std::string word;
        receiver.recv(&message);
        std::string smessage(static_cast<char*> (message.data()), message.size());
        
        //std::cout << smessage << std::endl;
        std::istringstream iss(smessage);

        int n = param->next_stage;

        StringArray * retmessage = NULL;
        int * retlen_a = NULL;
        int retlen = 0;

        notify_enclave_execute(smessage, n, &sender, &retmessage, &retlen_a, &retlen);

        for (int k = 0; k < retlen; k++) {
            message.rebuild(retlen_a[k]);

            snprintf((char *) message.data(), retlen_a[k], "%s", retmessage->array[k]);
            sender.send(message);
            std::cout << retmessage->array[k] << std::endl;
        }

    }
    return NULL;
}
