#include <string>
#include <msgpack.hpp>

struct Message{
	std::vector<std::string> value;
	std::vector<std::string> gcm_tag;
	std::vector<long> timeSec;
	std::vector<long> timeNSec;
        MSGPACK_DEFINE(value,gcm_tag, timeSec, timeNSec);
};
