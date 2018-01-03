#include <string>
#include <msgpack.hpp>

struct Message{
	std::vector<std::string> value;
//	#ifdef SGX
	std::vector<std::string> gcm_tag;
//	#endif
	std::vector<long> timeSec;
	std::vector<long> timeNSec;
//	#ifdef SGX
        MSGPACK_DEFINE(value,gcm_tag, timeSec, timeNSec);
//	#else
//	MSGPACK_DEFINE(value, timeSec, timeNSec);
//	#endif
};
