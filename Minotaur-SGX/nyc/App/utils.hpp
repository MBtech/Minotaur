/* 
 * File:   utils.hpp
 * Author: mb
 *
 * Created on May 29, 2017, 1:25 PM
 */

#ifndef UTILS_HPP
#define	UTILS_HPP



#endif	/* UTILS_HPP */
#include "zmq.hpp"
struct Arguments{
    const char * ip; 
    int id;
    int next_stage;
    int prev_stage;
};

long calLatency(long endtime, long endNtime, long startTime, long startNTime);

zmq::socket_t key_receiver_conn(Arguments * param, zmq::context_t & context, std::string ip, int port);
zmq::socket_t* key_sender_conn(Arguments * param, zmq::context_t & context, std::vector<std::string> ip, std::vector<int> port);
zmq::socket_t shuffle_receiver_conn(Arguments * param, zmq::context_t & context, 
        std::vector<std::string> ip, std::vector<int> port);
zmq::socket_t* shuffle_sender_conn(Arguments * param, zmq::context_t & context, std::string ip, int port);
