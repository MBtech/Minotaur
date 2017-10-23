#include <string>
#include <msgpack.hpp>

struct Message{
	std::vector<std::string> value;
	//std::vector<std::string> gcm_tag;
	long timeSec;
	long timeNSec;
        MSGPACK_DEFINE(value, timeSec, timeNSec);
};
