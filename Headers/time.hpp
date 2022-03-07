#ifndef UTP_TIME_H
#define UTP_TIME_H

#include <sstream>
#include <chrono>
#include <ctime>

namespace wfutp
{
	std::string GetTime()
	{
		//Get current time
		std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		//create stream object
		std::stringstream timestream;
		//load time to stream
		timestream << std::ctime(&now_time);
		//move data to passable object
		std::string timestring = timestream.str();
		if (timestring.back() == '\n')
		{
			timestring.pop_back();
		}
		return timestring;
	}
}
#endif
