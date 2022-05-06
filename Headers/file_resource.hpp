#ifndef UTP_FILE_RES_H
#define UTP_FILE_RES_H

#include <httpserver.hpp>
#include "srvlog.hpp"
#include <fstream>

class file_resource : public httpserver::http_resource
{
	protected:
		std::string content_path;
		std::string content_type;
		int upload_count = 0;

		file_resource(){};//disallow default constructor
	public:

		//useful constructor
		file_resource(std::string i_path, std::string i_type): 
			content_path(i_path), 
			content_type(i_type)
			{}

		// GET
		virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);
			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, content_type));
		}

		//PUT
		virtual const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			if(req.content_too_large())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
			}
			
			upload_count++;	//not thread-safe, but w/e
			std::ofstream output("uploaded_file" + std::to_string(upload_count) + ".txt");

			if(!output.is_open())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Error storing file", 500));
			}
			
			//content is NOT too large and output file opened properly
			output << req.get_content();
			output.close();
			
			srvlog.log_good(req);
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File uploaded successfully\n", 200));
				
		}



		//POST

		virtual const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			
			if(req.content_too_large())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
			}
			
			std::ofstream output("uploaded_file.txt", std::ios::app);
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

		//DELETE

		virtual const std::shared_ptr<httpserver::http_response> render_DELETE(const httpserver::http_request& req)
		{
			srvlog.log_fail(req);
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Delete not authorized", 403));
		}
};

class read_only_file_resource : public httpserver::http_resource
{
	protected:
		std::string content_path;
		std::string content_type;

		read_only_file_resource(){}//disallow default constructor
	public:

		//useful constructor
		read_only_file_resource(std::string i_path, std::string i_type = "text/plain"): 
			content_path(i_path), 
			content_type(i_type)
			{}

		// GET
		virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);
			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, content_type));
		}
};

// purpose: GET takes you to html form. PUT/POST load request contents to a file, then returns content of file
class form_resource : public file_resource
{
	protected:
		std::string html_path;
	public:
		form_resource(std::string i_html_path, std::string i_content_path): html_path(i_html_path), file_resource(i_content_path, "text/plain") {}

		// GET
		virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);
			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(html_path, 200, "text/html"));
		}


		//PUT
		virtual const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			if(req.content_too_large())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
			}
			

			std::ofstream output(content_path);
			if(!output.is_open())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Error storing file", 500));
			}
			
			//content is NOT too large and output file opened properly
			output << req.get_content();
			output.close();
			
			srvlog.log_good(req);
			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, "text/plain"));
				
		}



		//POST

		virtual const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			
			if(req.content_too_large())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
			}
			
			std::ofstream output(content_path, std::ios::app);
			if(!output.is_open())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Error storing file", 500));
			}
			
			//content is NOT too large and output file opened properly
			output << req.get_content();
			output.close();
			
			srvlog.log_good(req);
			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, "text/plain"));
		}

};


class status_resource : public file_resource
{
	public:
		status_resource(std::string i_path, std::string i_type): file_resource(i_path, i_type) {}
		// GET
		virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);
			std::ifstream input("/proc/self/status", std::ios::in);
			//std::ifstream input("trashfile.txt", std::ios::in);
			std::ofstream output(content_path);
			if((!input.is_open())||(!output.is_open()))
			{
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Error getting status", 500));
			}

			std::string data;
			while(std::getline(input, data))
			{
				output << data << std::endl;
			}


			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, content_type));
		}


};

class fetch_resource : public file_resource
{
	protected:
		//fetch_resouce():file_resource(){};//disallow default constructor

	public:
		fetch_resource(std::string i_path, std::string i_type):file_resource(i_path, i_type){}
		
		// GET
		virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			std::string grabarg = req.get_arg("fetch");
			if (grabarg == "users")
			{

			}

			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, content_type));
		}


		//PUT
		virtual const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			/*std::string grabarg = req.get_arg("delay");
			int test = fix(grabarg);


			int long resp_code;
			grabarg = req.get_arg("threads");
			int thread_count = fix(grabarg);
			grabarg = req.get_arg("repeats");
			int repeats = fix(grabarg);
			std::cout << "thread " << thread_count << std::endl;
			std::cout << "repeats " << repeats << std:: endl;

			int total = repeats * thread_count;
			*/
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
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File uploaded successfully\n", 200));
				
		}

};
#endif
