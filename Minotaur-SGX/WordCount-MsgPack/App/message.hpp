#include <string>
#include <msgpack.hpp>

struct Message{
	std::string value;
	std::string gcm_tag;
	long timeSec;
	long timeNSec;
        MSGPACK_DEFINE(value,gcm_tag,timeSec, timeNSec);
};
