/*
     This file is part of libhttpserver
     Copyright (C) 2011, 2012, 2013, 2014, 2015 Sebastiano Merlino

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
     USA
*/

#include <httpserver.hpp>
#include <thread>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

const int SMALL_CONSTANT = 2;


class config_keeper
{
	public:
	class config_proxy
	{
		private:
			bool can_make_num(std::string input)
			{
				//define this later
				return false;
			}

			int do_make_num(std::string input)
			{
				//also define this later
				return 0;
			}
		public:
		// I can add more datatypes to the proxy as needed
		// each datatype needs a member, a constructor, a set function, and a cast operator
		std::string str;
		int n;

		config_proxy(std::string input):str(input){}
		config_proxy(int input):n(input){}
		
		void set_explicit_string(std::string input)
		{
			str = input;
		}
		void set(std::string input)
		{
			if(can_make_num(input))
			{
				set(do_make_num(input));
			}
			else
			{
				str = input;
			}
		}
		void set(int input)
		{
			n = input;
		}

		operator std::string() const
		{
			return str;
		}
		operator int() const
		{
			return n;
		}
	};

	std::map<std::string, config_proxy> configs;
	config_keeper()
	{
		//might later need to find a way to add the pointers, but that seems unlikely
		configs.emplace("port", 80);
		configs.emplace("max_threads", 0);
		configs.emplace("max_connections", 0);
		configs.emplace("memory_limit", 0);
		configs.emplace("content_size_limit", -1);
		configs.emplace("connection_timeout", 2000);
		configs.emplace("per_IP_connection_limit", 0);
		//configs.emplace("log_access_ptr", nullptr);
		configs.emplace("bind_socket", 0);
		configs.emplace("max_thread_stack_size", 0);
		configs.emplace("use_ssl", true);
		configs.emplace("use_ipv6", false);
		configs.emplace("use_dual_stack", false);
		configs.emplace("debug", false);
		configs.emplace("pedantic", false);
		configs.emplace("https_mem_key", "");
		configs.emplace("https_mem_cert", "");
		configs.emplace("https_mem_trust", "");
		configs.emplace("https_priorities", "");
		configs.emplace("digest_auth_random", "");
		configs.emplace("nonce_nc_size", 0);
		configs.emplace("basic_auth_enabled", true);
		configs.emplace("digest_auth_enabled", true);
		configs.emplace("regex_checking", true);
		configs.emplace("ban_system_enabled", true);
		configs.emplace("post_process_enabled", true);
		configs.emplace("deferred_enabled", false);
		configs.emplace("single_resource", false);
		configs.emplace("tcp_nodelay", false);
	}
	void read_file()
	{
		std::ifstream configvals;
		configvals.open("config.txt");
		if(!configvals.is_open())
		{
			return;
		}
		std::string raw_data;
	//	std::stringstream data;
		std::string key;
		std::string equals;
		std::string value;
		std::map<std::string, config_proxy>::iterator iter;

		while(getline(configvals, raw_data))
		{
			std::stringstream data;
			data.str(raw_data);	//why was this not enough by itself. Why did I need to have data leave and re-enter scope to purge its existing data?
			data >> key >> equals >> value;

			iter = configs.find(key);
			if(iter != configs.end())
			{
				iter->second.set(value);
			}
		}
	}
};





class hello_world_resource : public httpserver::http_resource 
{
public:
	const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request&)
	{
        	return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Hello, World!\n"));
     	}
};

#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

class digest_resource : public httpserver::http_resource {
 public:
     const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req) {
         if (req.get_digested_user() == "") {
             return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque", "weston", MY_OPAQUE, true));
         } else {
             bool reload_nonce = false;
             if (!req.check_digest_auth("weston", "kitty", 300, &reload_nonce)) {
                 return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque-usr/pass", "weston", MY_OPAQUE, reload_nonce));
             }
         }
         return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Good server, have a cookie", 200, "text/plain"));
     }
};

void monitor_performance(bool& server_alive)
{
	std::cout << "performance monitoring called" << std::endl;
	std::ofstream log;
	log.open("log.txt");
	if(!log.is_open())
	{
		std::cout << "ERROR IN PERFORMANCE LOG" << std::endl;
		return;
	}

	log << "--PERFORMANCE LOG START --" << std::endl;

	while(server_alive)
	{
		//monitor performance here
	}
}



int main()
{
	//*******  SET CONFIGURATION 
	//httpserver::create_webserver serv(8080);
	//serv.use_dual_stack();
	//httpserver::webserver ws = serv
	
	bool server_alive = false;
	// NOTE: member functions like ".use_dual_stack()" are private in webserver, but NOT in create webserver, so these functions can be called as part of the webserver = create_webserver()... chain, but NOT on the webserver afterward.

	int max_threads = std::thread::hardware_concurrency() * SMALL_CONSTANT;
	httpserver::http::http_utils::start_method_T start_method = httpserver::http::http_utils::INTERNAL_SELECT;
	int timeout = 180;		//default value
	int memory_limit = 32 * 1024;	//default value of 32kb
	int max_conns = FD_SETSIZE - 4;
	//create the webserver via copy constructor of a create_webserver object. The functions in the chain following each return a reference to the create_webserver object that called them, enabling the chaining behavior seen here
	httpserver::webserver ws = httpserver::create_webserver(8080)
	.start_method(start_method)	// this is the default value, but I want it to be explicit
	.max_threads(max_threads)	// combining this with the start method, automatically implements a thread pool
	.use_ssl()
	.connection_timeout(timeout)
	.memory_limit(memory_limit)
	.max_connections(max_conns)
	.https_mem_key("newkey.pem")	// use different key later
	.https_mem_cert("newcert.crt")	// use difference cert later
	.use_dual_stack();


	hello_world_resource hwr;
	ws.register_resource("/hello", &hwr);
	digest_resource dgr;
	ws.register_resource("/dig", &dgr);

	std::thread servthread([&](){ws.start(true);});
	//will need a better system for setting server_alive to true or not, based on actual response from the server, but for now, hard-code it
	server_alive = true;

	std::thread performancethread(monitor_performance, std::ref(server_alive));
	
	//servthread should only join after server has stopped operating. Current version never stops operating except from a ctrl + C, which kills the whole program, but still...
	servthread.join();
	server_alive = false;
	//have not decided yet if performancethread should actually re-join or if it should detach. Can't detach if I want to issue it instructions, so for now, it expects joining
	performancethread.join();
	return 0;
}
