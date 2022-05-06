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

#include "headers/config.hpp"
#include "headers/srvlog.hpp"
#include "headers/file_resource.hpp"
#include "headers/act_resource.hpp"
#include "headers/awfulcurl.hpp"
#include "headers/query_resource.hpp"
/*
//#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"
#define MY_OPAQUE "11733c200778ce33060f31c9af70a870ba96ddd4"

class digest_resource : public httpserver::http_resource {
 public:
     const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req) {
         if (req.get_digested_user() == "") {
             return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque", "demo", MY_OPAQUE, true));
         } else {
             bool reload_nonce = false;
             if (!req.check_digest_auth("demo", "demo", 300, &reload_nonce)) {
                 return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque-usr/pass", "demo", MY_OPAQUE, reload_nonce));
             }
         }
         return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Credentials Accepted", 200, "text/plain"));
     }
};
*/
//use log of throughput data to identify, highlight, and resolve bottlenecks.
void log_throughput()
{
	//call tools or equivalent:
	//fio(1)	
	
	//with local storage:
	{

		// disk stats

		// IO type

		// block size

		// I/O size

		// I/O ops (iops)

		// I/O rate

		//disk I/O cache

		// I/O latency
	}

	//with network storage (NAS):	//amazon S3 bucket. possibly do another set with a different type of NAS? maybe via LAN?
	{

		// disk stats

		// IO type

		// block size

		// I/O size

		// I/O ops (iops)

		// I/O rate
	}

	//print results to log file

}
void monitor_performance(bool& server_alive)
{
	bool log_now = false;
	//int i_period, bool& i_blinker, std::chrono::duration i_units = std::chrono::milliseconds
/*	pulsar monitor_interval(5000, log_now)	// set log_now true 5000 ms after starting monitor_interval
	while (server_alive)
	{
		log_throughput();			//log thoughput data
		monitor_interval.Wait();	//wait at least 5 seconds
	}
	*/
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
	config_keeper configs;
	bool server_alive = false;
	// NOTE: member functions like ".use_dual_stack()" are private in webserver, but NOT in create webserver, so these functions can be called as part of the webserver = create_webserver()... chain, but NOT on the webserver afterward.
	
	/*
	const int SMALL_CONSTANT = 2;
	int max_threads = std::thread::hardware_concurrency() * SMALL_CONSTANT;
	httpserver::http::http_utils::start_method_T start_method = httpserver::http::http_utils::INTERNAL_SELECT;
	int timeout = 180;		//default value
	int memory_limit = 32 * 1024;	//default value of 32kb
	int max_conns = FD_SETSIZE - 4;
	std::string key_location = "newkey.pem";
	std::string cert_location = "newcert.crt";
	*/

	httpserver::create_webserver cws(8080);
	httpserver::webserver ws = configs.ConfigureWebserver(cws);

	/*
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
	*/

	digest_resource dgr;
	ws.register_resource("/dig", &dgr);

	//all files referenced here are present in the folder from which I am running the utopia server, even if I'm not copying them to Github. "html.txt" calls the image from the server itself though, at /content/ralph


	file_resource flr1("example_1.png", "image/png");	//the constructor parameters don't matter if I'm making PUT/POST requests
	ws.register_resource("/content", &flr1);

	file_resource flr2("example_1.png", "image/png");
	file_resource flr3("example_2.jpg", "image/jpg");
	ws.register_resource("/content/example_1", &flr2);
	ws.register_resource("/content/example_2", &flr3);

	file_resource flr7("/home/tiny.txt", "text/plain");
	ws.register_resource("/punchbag", &flr7);

	form_resource frs("form/formhtml.html", "form/formtext.txt");
	ws.register_resource("/index", &frs);
	
	file_resource usr("/home/credit_card/credit_card/outuser.txt", "text/plain");
	ws.register_resource("/user", &usr);	
	
	file_resource merch("/home/credit_card/credit_card/outmerch.txt", "text/plain");
	ws.register_resource("/merch", &merch);

	file_resource state("/home/credit_card/credit_card/outstate.txt", "text/plain");
	ws.register_resource("/state", &state);

	config_resource cfg("config.html", "config.txt", configs);
	ws.register_resource("/config", &cfg);
	
	read_only_file_resource rofr("logs/log.txt");
	ws.register_resource("/log", &rofr);

	status_resource stsr("status.txt", "text/plain");
	ws.register_resource("/status", &stsr);

	active_resource actr{};
	ws.register_resource("/act", &actr);

	quit_resource quitter{ &ws };
	ws.register_resource("quit", &quitter);

	button_resource but("form/buttons.html");
	ws.register_resource("/reports", &but);

	query_resource qry("query/reports/");
	//query_resource qry("tests/testdata results/reports/");	//actual report location is busy with being made
	ws.register_resource("/query", &qry);

	std::thread servthread([&]() {ws.start(true); });
	configs.WriteFile();
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
