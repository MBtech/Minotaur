// classes example
#include "TimedBuffer.hpp"
#include "message.hpp"
#include "zhelpers.hpp"

TimedBuffer::TimedBuffer (zmq::context_t * c,zmq::socket_t* s, long timeout) {
    buffer_timeout = timeout;
    context = c ;
    sender = s; 
    //*sender = shuffle_sender_conn(param, *context, ip, port);
    message = new zmq::message_t(1);
    timeval startTime;
    gettimeofday(&startTime, NULL);
    startMicro = startTime.tv_sec*(uint64_t)1000000+startTime.tv_usec;
}

void TimedBuffer::add_msg (int dest, std::string data, std::string gcm, long tsec, long tnsec) {
    //std::cout << "Sending to: " << dest << std::endl;
    data_vector[dest].push_back(data);
    gcm_vector[dest].push_back(gcm);
    tsec_vector[dest].push_back(tsec);
    tnsec_vector[dest].push_back(tnsec);
}

void TimedBuffer::check_and_send() {
    timeval curTime;
    gettimeofday(&curTime, NULL);
    unsigned long curMicro = curTime.tv_sec*(uint64_t)1000000+curTime.tv_usec;
    if(curMicro-startMicro >= buffer_timeout) {
        map<int, std::vector<std::string>>::iterator it, it1;
	map<int, std::vector<long>>::iterator it2, it3;
        for(it = data_vector.begin(), it1=gcm_vector.begin(), it2=tsec_vector.begin(), it3=tnsec_vector.begin(); it != data_vector.end(); ++it, ++it1,++it2, ++it3) {
            Message msg;
            msg.value= it->second;
            msg.gcm_tag = it1->second;

            msg.timeNSec = it2->second;
            msg.timeSec = it3->second;

            msgpack::sbuffer packed;
            msgpack::pack(&packed, msg);

            message->rebuild(packed.size());
            std::memcpy(message->data(), packed.data(), packed.size());

            s_sendmore(*sender, std::to_string(it->first));
            sender->send(*message);
        }
        startMicro = curMicro;

        data_vector.clear();
        gcm_vector.clear();
        tsec_vector.clear();
        tnsec_vector.clear();
   }

}
