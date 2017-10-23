#include "zhelpers.hpp"
#include <string>
#include <functional>
#include "utils.hpp"

long calLatency(long endTime, long endNTime, long startTime, long startNTime){
	long lat = 0;
	if (endTime>startTime)
		lat = endNTime + (1000000-startNTime);
	else
		lat = endNTime - startNTime;
	
	return lat/1000;
}
// Function to setup connection for a receiver endpoint of a key based grouping
zmq::socket_t key_receiver_conn(Arguments * param, zmq::context_t & context, std::string ip, int port){
    //  Socket to receive messages on
    zmq::socket_t receiver(context, ZMQ_SUB);
    std::cout << "Connecting to port " << port+param->id<< " IP "<< ip << std::endl;
    receiver.bind("tcp://"+ip+":"+std::to_string(port+param->id));
    const char * filter = std::to_string(param->id).c_str();
    receiver.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
    return receiver;
}

// Function to setup connection for a sender endpoint of a key based grouping
zmq::socket_t* key_sender_conn(Arguments * param, zmq::context_t & context, 
        std::vector<std::string> ip, std::vector<int> port){
    //  Socket to send messages to
    zmq::socket_t* sender = new zmq::socket_t(context, ZMQ_PUB);
    for (int i = 0; i<port.size();i++){
        sender->connect("tcp://"+ip[i]+":"+std::to_string(port[i]));
    std::cout << "Connecting to port " << port[i] << " IP "<< ip[i] << std::endl;
    }
    return sender;
}

// Function to setup connection for a sender endpoint of a shuffle based grouping
zmq::socket_t* shuffle_sender_conn(Arguments * param, zmq::context_t & context, std::string ip, int port){
    //  Socket to send messages on
    zmq::socket_t*  sender = new zmq::socket_t(context, ZMQ_PUB);
    //std::cout << "Binding to " << "tcp://127.0.0.1:"+std::to_string(5000+param->id) << std::endl;
    std::cout << "Connecting to port " << port+param->id << " IP "<< ip << std::endl;
    sender->bind("tcp://"+ip+":"+std::to_string(port+param->id));
    return sender;
}

// Function to setup connection for a receiver endpoint of a shuffle based grouping
zmq::socket_t shuffle_receiver_conn(Arguments * param, zmq::context_t & context, 
        std::vector<std::string> ip, std::vector<int> port){
    zmq::socket_t receiver(context, ZMQ_SUB);
    for (int i = 0; i<port.size();i++){
    std::cout << "Connecting to port " << port[i] << " IP "<< ip[i] << std::endl;
        receiver.connect("tcp://"+ip[i]+":"+std::to_string(port[i]));
    }
    
    const char * filter = std::to_string(param->id).c_str();
    receiver.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
    return receiver;
}
