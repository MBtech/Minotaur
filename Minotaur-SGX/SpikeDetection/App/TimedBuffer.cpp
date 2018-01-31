// classes example
#include "TimedBuffer.hpp"
#include "message.hpp"
#include "zhelpers.hpp"
#include "user_types.h"

TimedBuffer::TimedBuffer (zmq::context_t * c, std::vector<zmq::socket_t*> s, long timeout) {
    buffer_timeout = timeout;
    context = c ;
    sender = s; 
    //*sender = shuffle_sender_conn(param, *context, ip, port);
    message = new zmq::message_t(1);
    timeval startTime;
    gettimeofday(&startTime, NULL);
    startMicro = startTime.tv_sec*(uint64_t)1000000+startTime.tv_usec;
}

void TimedBuffer::add_msg (int stream, int dest, std::string data, std::string gcm, long tsec, long tnsec) {
    data_vector[stream][dest].push_back(data);
    gcm_vector[stream][dest].push_back(gcm);
    tsec_vector[stream][dest].push_back(tsec);
    tnsec_vector[stream][dest].push_back(tnsec);
}

void TimedBuffer::add_msg (int stream, int dest, std::string data, long tsec, long tnsec) {
    //std::cout << "Sending to: " << dest << std::endl;
    data_vector[stream][dest].push_back(data);
    tsec_vector[stream][dest].push_back(tsec);
    tnsec_vector[stream][dest].push_back(tnsec);
}

void TimedBuffer::check_and_send(bool send) {
    timeval curTime;
    gettimeofday(&curTime, NULL);
    unsigned long curMicro = curTime.tv_sec*(uint64_t)1000000+curTime.tv_usec;
    struct timespec tv;
    if((curMicro-startMicro >= buffer_timeout) || send) {
//    std::cout << "Time diff: " << curMicro-startMicro << std::endl;
      map<int, map<int, std::vector<std::string>>>::iterator baseit;
      int i=0;
      for(baseit = data_vector.begin(); baseit != data_vector.end(); ++baseit, ++i){
        map<int, std::vector<std::string>>::iterator it, it1;
	map<int, std::vector<long>>::iterator it2, it3;
        int streamid = baseit->first;
	#ifdef SGX
	it1 = gcm_vector[streamid].begin();
	#endif
        for(it = data_vector[streamid].begin(), it2=tsec_vector[streamid].begin(), it3=tnsec_vector[streamid].begin(); it != data_vector[streamid].end(); ++it, ++it2, ++it3) {
            Message msg;
            msg.value= it->second;
	    #ifdef SGX
            msg.gcm_tag = it1->second;
	    #endif

            clock_gettime(CLOCK_REALTIME, &tv);
            std::vector<long> vec(it2->second.size(), tv.tv_sec);
            msg.timeSec = vec;
            std::vector<long> vec1(it3->second.size(), tv.tv_nsec);
            msg.timeNSec = vec1;

            msgpack::sbuffer packed;
            msgpack::pack(&packed, msg);

            message->rebuild(packed.size());
            std::memcpy(message->data(), packed.data(), packed.size());
	
            s_sendmore(*(sender[streamid]), std::to_string(it->first));
            sender[streamid]->send(*message);
	    #ifdef SGX
	    ++it1;
	    #endif
            if(!send){
            for(int i=0; i<it2->second.size(); i++){
            long latency = calLatency(tv.tv_sec, tv.tv_nsec, it2->second[i], it3->second[i]);
	    //std::cout << "Buffer Latency: " << latency<<std::endl;
	    std::cout << "Latency:" << latency<<std::endl;
   		}
            }
        }
	}
        startMicro = curMicro;

        data_vector.clear();
	#ifdef SGX
        gcm_vector.clear();
	#endif
        tsec_vector.clear();
        tnsec_vector.clear();
   }

}
