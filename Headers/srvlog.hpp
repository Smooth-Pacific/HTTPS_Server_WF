#ifndef SERV_LOG_H
#define SERV_LOG_H

#include <fstream>
#include <httpserver.hpp>

#include "time.hpp"

class log_not_open : public std::exception
{
	public:
	const char * what() const throw()
	{
		return "Server log file could not be opened";
	}
};


class serv_log_bot
{
	private:
		std::string filepath;
		std::string filename;
		std::ofstream output;

		serv_log_bot(){};//default constructor not available
	public:
		//constructors
		serv_log_bot(std::string i_filepath, std::string i_filename): filepath(i_filepath), filename(i_filename), output(i_filepath + i_filename, std::ios::app)
		{
			startup_and_check();
		}

		//delete copy constructor and assignment operators
		serv_log_bot(const serv_log_bot& source) = delete;
		serv_log_bot& operator =(const serv_log_bot& source) = delete;

		//destructor
		~serv_log_bot()
		{
			if(output.is_open())
			{	
				output << "[NOTE] " << wfutp::GetTime() << "\n" << "Logging session end\n" << std::endl;
			}
			output.close();
		}

	private:
		//open log file, throw an exception if it did not open correctly. Do not call outside the one in the constructor
		void startup_and_check() 
		{
//			output.open(filepath + filename, std::ios::app);

			if(!output.is_open()) throw log_not_open();
			output << "[NOTE] " << wfutp::GetTime() << "\n" << "Logging session start\n" << std::endl;
		}

	public:
		void log_good(const httpserver::http_request& req)
		{
			output << "[GOOD] " << wfutp::GetTime() << "\n" << req << std::endl;
		}
		void log_fail(const httpserver::http_request& req)
		{
			output << "[FAIL] " << wfutp::GetTime() << "\n" << req << std::endl;
		}
		void log_note(const httpserver::http_request& req) 
		{
			output << "[NOTE] " << wfutp::GetTime() << "\n" << req << std::endl;
		}
};


serv_log_bot srvlog("logs/", "log.txt");
#endif
