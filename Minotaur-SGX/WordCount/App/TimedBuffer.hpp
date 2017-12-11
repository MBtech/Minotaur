// classes example
#include <iostream>
#include <msgpack.hpp>
#include <map>
#include "zhelpers.hpp"
#include "zmq.hpp"
#include <time.h>
#include "utils.hpp"
using namespace std;

class TimedBuffer {
    std::map<int, std::vector<std::string>> data_vector, gcm_vector;
    std::map<int, std::vector<long>> tsec_vector, tnsec_vector;
    unsigned long startMicro;
    zmq::context_t* context = new zmq::context_t(1);
    zmq::socket_t* sender = new zmq::socket_t(*context, ZMQ_PAIR);
    zmq::message_t* message;
    long buffer_timeout;
public:
    TimedBuffer(zmq::context_t * context, zmq::socket_t * s, long timeout);
    void check_and_send();
    void add_msg(int, std::string, std::string, long, long);
    void add_msg(int, std::string, long, long);

};
