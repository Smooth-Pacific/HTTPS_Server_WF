#ifndef UTP_FILE_RES_H
#define UTP_FILE_RES_H

#include<httpserver.hpp>
#include "srvlog.hpp"

class file_resource : public httpserver::http_resource
{
	private:
		std::string content_path;
		std::string content_type;

		file_resource(){};//disallow default constructor
	public:

		//useful constructor
		file_resource(std::string i_path, std::string i_type): 
			content_path(i_path), 
			content_type(i_type)
			{}

		// GET
		const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);
			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, content_type));
		}

		//PUT
		const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			std::ofstream output("uploaded_file.txt");
			
			if(req.content_too_large())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
			}
			
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



		//POST

		const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& req)
		{
			srvlog.log_note(req);

			std::ofstream output("uploaded_file.txt", std::ios::app);
			
			if(req.content_too_large())
			{
				srvlog.log_fail(req);
				return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
			}
			
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

		const std::shared_ptr<httpserver::http_response> render_DELETE(const httpserver::http_request& req)
		{
			srvlog.log_fail(req);
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Delete not authorized", 403));
		}
};




#endif
