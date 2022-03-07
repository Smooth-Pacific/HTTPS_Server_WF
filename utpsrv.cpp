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

#include "Headers/config.hpp"
#include "Headers/srvlog.hpp"
#include "Headers/file_resource.hpp"

#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

class digest_resource : public httpserver::http_resource {
 public:
     const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req) {
         if (req.get_digested_user() == "") {
             return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque", "weston", MY_OPAQUE, true));
         } else {
             bool reload_nonce = false;
             if (!req.check_digest_auth("weston", "demo", 300, &reload_nonce)) {
                 return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque-usr/pass", "weston", MY_OPAQUE, reload_nonce));
             }
         }
         return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Good server, have a cookie", 200, "text/plain"));
     }
};

void monitor_performance(bool& server_alive)
{
/*	std::cout << "performance monitoring called" << std::endl;
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
*/}



int main()
{
	//*******  SET CONFIGURATION 
	//httpserver::create_webserver serv(8080);
	//serv.use_dual_stack();
	//httpserver::webserver ws = serv
	
	bool server_alive = false;
	// NOTE: member functions like ".use_dual_stack()" are private in webserver, but NOT in create webserver, so these functions can be called as part of the webserver = create_webserver()... chain, but NOT on the webserver afterward.

	const int SMALL_CONSTANT = 2;
	int max_threads = std::thread::hardware_concurrency() * SMALL_CONSTANT;
	httpserver::http::http_utils::start_method_T start_method = httpserver::http::http_utils::INTERNAL_SELECT;
	int timeout = 180;		//default value
	int memory_limit = 32 * 1024;	//default value of 32kb
	int max_conns = FD_SETSIZE - 4;
	std::string key_location = "newkey.pem";
	std::string cert_location = "newcert.crt";
	
	//create the webserver via copy constructor of a create_webserver object. The functions in the chain following each return a reference to the create_webserver object that called them, enabling the chaining behavior seen here
	httpserver::webserver ws = httpserver::create_webserver(8080)
	.start_method(start_method)	// this is the default value, but I want it to be explicit
	.max_threads(max_threads)	// combining this with the start method, automatically implements a thread pool
	.use_ssl()
	.connection_timeout(timeout)
	.memory_limit(memory_limit)
	.max_connections(max_conns)
	.https_mem_key(key_location)
	.https_mem_cert(cert_location)
	.use_dual_stack();


	digest_resource dgr;
	ws.register_resource("/dig", &dgr);
//	file_read_resource frr;
//	ws.register_resource("/content", &frr);
	
	//all files referenced here are present in the folder from which I am running the utopia server, even if I'm not copying them to Github. "html.txt" calls the image from the server itself though, at /content/ralph
//	file_resource flr("content.txt", "text/plain");
//	ws.register_resource("/content", &flr);
	file_resource flr2("ralph.png","image/png");
	file_resource flr3("endpoint.jpg", "image/jpg");
	ws.register_resource("/content/ralph", &flr2);
	ws.register_resource("/content/endpoint", &flr3);
	file_resource flr4("html.txt", "text/html");
	ws.register_resource("/content", &flr4);

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
