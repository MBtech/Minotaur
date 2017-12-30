/* 
 * File:   utils.hpp
 * Author: mb
 *
 * Created on May 29, 2017, 1:25 PM
 */

#ifndef UTILS_HPP
#define	UTILS_HPP


#include "zmq.hpp"
#include "user_types.h"
struct Arguments{
    const char * ip; 
    int id;
    int windowSize;
    int next_stage;
    int prev_stage;
    std::vector<std::string> senderIP, receiverIP;
    std::vector<int> senderPort, receiverPort;
    std::vector<std::string> in_grouping, out_grouping; 
};
struct Sockets{
       zmq::socket_t* sender;
       zmq::socket_t* receiver;
};
typedef struct Sockets Sockets;

long calLatency(long endtime, long endNtime, long startTime, long startNTime);

void zmq_init(void * arg, zmq::context_t* context, Sockets* socks, int* forward, int* back );

zmq::socket_t* key_receiver_conn(Arguments * param, zmq::context_t & context, std::string ip, int port);

zmq::socket_t* key_sender_conn(Arguments * param, zmq::context_t & context, std::vector<std::string> ip, std::vector<int> port);
zmq::socket_t* shuffle_receiver_conn(Arguments * param, zmq::context_t & context, 
        std::vector<std::string> ip, std::vector<int> port);
zmq::socket_t* shuffle_sender_conn(Arguments * param, zmq::context_t & context, std::string ip, int port);

#endif	/* UTILS_HPP */
