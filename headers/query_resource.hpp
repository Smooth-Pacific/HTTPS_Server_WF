#ifndef UTP_QUERY_RES_H
#define UTP_QUERY_RES_H

#include <httpserver.hpp>
#include "srvlog.hpp"
#include <fstream>

//#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"
#define MY_OPAQUE "11733c200778ce33060f31c9af70a870ba96ddd4"

class digest_resource : public httpserver::http_resource {
public:
	const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req) {
		if (req.get_digested_user() == "") {
			return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque", "demo", MY_OPAQUE, true));
		}
		else {
			bool reload_nonce = false;
			if (!req.check_digest_auth("demo", "demo", 300, &reload_nonce)) {
				return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque-usr/pass", "demo", MY_OPAQUE, reload_nonce));
			}
		}
		return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Credentials Accepted", 200, "text/plain"));
	}
};

// purpose: GET takes you to html form. PUT/POST have no meaning
class button_resource : public httpserver::http_resource
{
protected:
	std::string html_path;
public:
	button_resource(std::string i_html_path) : html_path(i_html_path) {}

	// GET
	virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
	{
		srvlog.log_note(req);
		/*
		if (req.get_digested_user() == "") {
			return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque", "demo", MY_OPAQUE, true));
		}
		else {
			bool reload_nonce = false;
			if (!req.check_digest_auth("demo", "demo", 300, &reload_nonce)) {
				return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL-opaque-usr/pass", "demo", MY_OPAQUE, reload_nonce));
			}
		}*/
		return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(html_path, 200, "text/html"));
	}
		
};


class query_resource : public httpserver::http_resource
{
	protected:
		std::string content_path;
		std::string content_type;

		query_resource(){};//disallow default constructor
	public:

		//useful constructor
		query_resource(std::string i_path):content_path(i_path), content_type("text/plain"){}

		// GET
		virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
		{
			srvlog.log_note(req);
			if (true)
			{
				//std::string filename = "transactions after 8pm with a value over $100.XML";
				std::string filename = req.get_arg("report");
				std::string filepath = content_path + filename;
				return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(filepath, 200, content_type));
			}
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("file not found", 404));
		}



		

};

#endif
