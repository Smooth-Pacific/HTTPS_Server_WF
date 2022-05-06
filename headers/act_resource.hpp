#ifndef UTP_ACT_RES_H
#define UTP_ACT_RES_H

#include <httpserver.hpp>
#include <fstream>
#include <thread>
#include <chrono>
#include <cmath>


#include "awfulcurl.hpp"
/*#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"
class quit_resource : public httpserver::http_resource {
 public:
	 httpserver::webserver* ws;
	 
	 quit_resource(httpserver::webserver* i_ws) : ws(i_ws) {}
     const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req) {
         if (req.get_digested_user() == "") {
             return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque", "weston", MY_OPAQUE, true));
         } else {
             bool reload_nonce = false;
             if (!req.check_digest_auth("demo", "demo", 300, &reload_nonce)) {
                 return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque-usr/pass", "weston", MY_OPAQUE, reload_nonce));
             }
         }
		 ws->sweet_kill();
         return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Server shutting down...", 200, "text/plain"));
     }
};
*/

class quit_resource : public httpserver::http_resource {
public:
	httpserver::webserver* ws;

	quit_resource(httpserver::webserver* i_ws) : ws(i_ws) {}
	const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req) 
	{		
		ws->sweet_kill();
		return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Server shutting down...", 200, "text/plain"));
	}
};
int fix(std::string data)
{
	int accumulator = 0;
	int length = data.length();
	for(int i = 0; i < length; i++)
	{
		if((data[i] < '0') || (data[i]) > '9')
		{
			return 0;
		}
		accumulator += ((data[i] - '0') * (pow(10, ((length - 1) - i))));
	}
	return accumulator;
}

void abuseserver(std::string target, long* resp_code, int* count)
{
	//*passes = 0;
	for(int i = 0; i < *count; i++)
	{
		curlGET(target, resp_code);
		//if(*resp_code == 200)
		//{
		//	passes++;	
		//}
		std::cout << *resp_code << '\n';
	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}

class active_resource : public httpserver::http_resource
{
	protected:
		std::string content_path;
		std::string content_type;

		
	public:
		active_resource(){}

		//useful constructor
		active_resource(std::string i_path, std::string i_type): 
			content_path(i_path), 
			content_type(i_type)
			{}

		// GET
		virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);
			
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Test Started", 200));
		}


/*
		//PUT
		virtual const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			if(req.content_too_large())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
			}
			

			std::ofstream output("uploaded_file.txt");
			if(!output.is_open())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Error storing file", 500));
			}
			
			//content is NOT too large and output file opened properly
			output << req.get_content();
			output.close();
			
			srvlog.log_good(req);
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File uploaded successfully", 200));
				
		}

*/

		//POST

		virtual const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			
			std::string grabarg = req.get_arg("delay");
			int test = fix(grabarg);


			int long resp_code;
			grabarg = req.get_arg("threads");
			int thread_count = fix(grabarg);
			grabarg = req.get_arg("repeats");
			int repeats = fix(grabarg);
			std::cout << "thread " << thread_count << std::endl;
			std::cout << "repeats " << repeats << std:: endl;

			int total = repeats * thread_count;
			std::vector<int> counts{thread_count, repeats};
			std::vector<int> passes{thread_count, 0};
			std::vector<std::thread> threads;
			
std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
			
			
			for(int i = 0; i < thread_count; i++)
{
	threads.push_back(std::thread(abuseserver, "https://76.121.52.252:8080/punchbag", &resp_code, &counts[i]));
}
			for(int i = 0; i < thread_count; i++)
{
	threads[i].join();
}

std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

			float duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
			duration /= 1000000;
/*
			int pass = 0;
			for(int i = 0; i < repeats; i++)
{
	std::cout << "passes " << i << " " << passes[i] << std::endl;
	pass += passes[i];
}
			
			std::cout << "There were " << pass << " successful connections in " << total << " attempts across a duration of " << duration << " seconds" << std::endl;
*/
std::cout << "Time elapsed " << duration << " seconds" << std::endl;
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File uploaded successfully", 200));
				
		}
/*
		//DELETE

		virtual const std::shared_ptr<httpserver::http_response> render_DELETE(const httpserver::http_request& req)
		{
			srvlog.log_fail(req);
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Delete not authorized", 403));
		}
*/
};


#endif
