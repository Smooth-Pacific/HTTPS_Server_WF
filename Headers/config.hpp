#ifndef UTP_CONFIG_H
#define UTP_CONFIG_H

#include <iostream>
#include <fstream>

#include "file_resource.hpp"


class config_proxy
{
private:

public:
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


	// I can add more datatypes to the proxy as needed
	// each datatype needs a member, a constructor, a set function, and a cast operator
	std::string str;
	int n;
	bool intout;
	config_proxy() :intout(true) {}
	config_proxy(std::string input) :str(input), intout(false) {}
	config_proxy(int input) :n(input), intout(true) {}

	void set_explicit_string(std::string input)
	{
		str = input;
		intout = false;
	}

	void set(std::string input)
	{
		if (can_make_num(input))
		{
			set(do_make_num(input));
		}
		else
		{
			str = input;
			intout = false;
		}
	}
	void set(int input)
	{
		n = input;
		intout = false;
	}

	operator std::string() const
	{
		return str;
	}
	operator int() const
	{
		return n;
	}
	operator httpserver::http::http_utils::start_method_T() const
	{
		/*if (n)
		{
			return httpserver::http::http_utils::THREAD_PER_CONNECTION;
		}*/

		return httpserver::http::http_utils::INTERNAL_SELECT;
	}
	/*
	operator int() const
	{
		return n;
	}
	operator int() const
	{
		return n;
	}*/


	friend std::ostream& operator<<(std::ostream& os, const config_proxy prox);
};

std::ostream& operator<<(std::ostream& os, const config_proxy prox)
{
	if (prox.intout)
	{
		os << static_cast<int>(prox.n);
	}
	else
	{
		os << prox.str;
	}
	return os;
}



class config_keeper
{


public:

	int parse_errors;
	std::string textfile_name;
	std::map<std::string, config_proxy> configs;
	config_keeper(std::string i_textfile_name = "config.txt") : textfile_name(i_textfile_name), parse_errors(0)
	{
		configs.emplace("per_IP_connection_limit", 0);
		configs.emplace("port", 8080);
		//configs.emplace("socket", <int>);					//do not force a default value
		configs.emplace("max_thread_stack_size", 0);		//zero means using the system default
		configs.emplace("use_ipv6", true);
		configs.emplace("use_dual_stack", true);
		configs.emplace("pedantic", false);
		configs.emplace("debug", false);
		configs.emplace("regex_checking", true);
		configs.emplace("post_process_enabled", true);
		/*
		//	.put_processed_data_to_content() and .no_put_processed_data_to_content()		//Enables / Disables the library to copy parsed body data to the content or to only store it in the arguments map.on by default.
			.file_upload_target(file_upload_target_T file_upload_target)		//Controls, how the library stores uploaded files.Default value is FILE_UPLOAD_MEMORY_ONLY.
					//FILE_UPLOAD_MEMORY_ONLY		//The content of the file is only stored in memory.Depending on put_processed_data_to_content only as part of the arguments map or additionally in the content.
					//FILE_UPLOAD_DISK_ONLY		//The content of the file is stored only in the file system.The path is created from file_upload_dir and either a random name(if generate_random_filename_on_upload is true) or the actually uploaded file name.
					//FILE_UPLOAD_MEMORY_AND_DISK		//The content of the file is stored in memory and on the file system.
		*/
		configs.emplace("start_method", httpserver::http::http_utils::INTERNAL_SELECT);
		const int SMALL_CONSTANT = 2;
		configs.emplace("max_threads", std::thread::hardware_concurrency() * SMALL_CONSTANT);
		configs.emplace("use_ssl", true);
		configs.emplace("connection_timeout", 2000);
		configs.emplace("key_location", "key.pem");
		configs.emplace("cert_location", "cert.crt");
		configs.emplace("max_connections", FD_SETSIZE - 4);
		configs.emplace("memory_limit", 32 * 1024);	//default value of 32kb
		configs.emplace("content_size_limit", -1);
		//configs.emplace("log_access_ptr", nullptr);
		configs.emplace("https_mem_trust", "");
		configs.emplace("https_priorities", "");
		configs.emplace("digest_auth_random", "");
		configs.emplace("nonce_nc_size", 0);
		configs.emplace("basic_auth_enabled", true);
		configs.emplace("digest_auth_enabled", true);
		configs.emplace("ban_system_enabled", true);
		configs.emplace("deferred_enabled", false);
		configs.emplace("single_resource", false);
		configs.emplace("tcp_nodelay", false);
	}
	void ReadFile()
	{
		parse_errors = 0;
		std::ifstream configvals;
		configvals.open("config.txt");
		if(!configvals.is_open())
		{
			std::cout << "Error opening config.txt\nConfiguration settings were not loaded" << std::endl;
			return;
		}
		std::string raw_data;
		//std::stringstream data;
		std::string key;
		std::string equals;
		std::string value;
		std::map<std::string, config_proxy>::iterator iter;

		while(getline(configvals, raw_data))
		{
			std::stringstream data;
			data.str(raw_data);			//why was this not enough by itself? Why did I need to have data leave and re-enter scope to purge its existing data?
			data >> key >> equals >> value;

			if (equals != "=")
			{
				parse_errors++;
			}
			else
			{
				if (!checkset_enum(key, value))
				{
					iter = configs.find(key);
					if (iter != configs.end())
					{
						iter->second.set(value);
					}
				}
			}	// end if/else (equals != "=")
		}	//end while(getline(configvals, raw_data))
		
		if (parse_errors > 0)
		{
			std::cout << "Parse errors: " << parse_errors << std::endl;
		}

	}

	void WriteFile()
	{
		std::ofstream configvals;
		configvals.open(textfile_name);
		if (!configvals.is_open())
		{
			std::cout << "Error opening config.txt\nConfiguration settings were not saved" << std::endl;
		}
		//std::map<std::string, config_proxy> configs;
		for (auto i = configs.begin(); i != configs.end(); i++)
		{
			configvals << i->first << "=" << i->second << std::endl;
		}
	}
	public:

	httpserver::create_webserver& ConfigureWebserver(httpserver::create_webserver& ws);
	//httpserver::create_webserver& ConfigureWebserver(httpserver::create_webserver& ws, int port);
	
	~config_keeper()
	{
		//WriteFile();
	}



	bool checkset_enum(std::string key, std::string input)
	{

		//defined in http_utils.hpp; namespace httpserver::http::http_utils
		//enum start_method_T {
			//INTERNAL_SELECT = MHD_USE_SELECT_INTERNALLY | MHD_USE_AUTO,
			//THREAD_PER_CONNECTION = MHD_USE_THREAD_PER_CONNECTION | MHD_USE_AUTO
		//policy_T{
					   //ACCEPT,
					  //REJECT
		//IP_version_T {
					   //IPV4 = 4,
					   //IPV6 = 1

		if (key == "start_method")
		{
			if ((input == "THREAD_PER_CONNECTION") || (input == "httpserver::http::http_utils::THREAD_PER_CONNECTION") || (input == "1"))
			{
				configs["start_method"] = httpserver::http::http_utils::THREAD_PER_CONNECTION;
			}
			else
			{
				configs["start_method"] = httpserver::http::http_utils::INTERNAL_SELECT;
			}
		}
		else if (key == "policy_T")
		{
			if ((input == "ACCEPT") || (input == "httpserver::http::http_utils::ACCEPT") || (input == "0"))
			{
				configs.emplace("policy_T", 0);
			}
			else if ((input == "REJECT") || (input == "httpserver::http::http_utils::REJECT") || (input == "1"))
			{
				configs.emplace("policy_T", 1);
			}
			//if neither is exact match, ignore and trust server to default
		}
		else if (key == "IP_version_T")
		{//IP_version_T {
					   //IPV4 = 4,
					   //IPV6 = 1
			if ((input == "IPV4") || (input == "httpserver::http::http_utils::IPV4") || (input == "0"))
			{
				configs.emplace("IP_version_T", 0);
			}
			else if ((input == "IPV6") || (input == "httpserver::http::http_utils::IPV6") || (input == "1"))
			{
				configs.emplace("IP_version_T", 1);
			}

		}
		else
		{
			return true;	//it was an enum and set the value
		}

		return false; //it was not an enum and needs further processing
	}

	void ResetToDefault()
	{
		std::ofstream configvals;
		configvals.open(textfile_name);
		configvals.close();

	}
};



httpserver::create_webserver& config_keeper::ConfigureWebserver(httpserver::create_webserver& ws)
{
	
	//httpserver::create_webserver cws = config_keeper::create_webserver(port);
	//ws.start_method(configs["start_method"]);	// this is the default value, but I want it to be explicit
	ws.start_method(httpserver::http::http_utils::INTERNAL_SELECT);	// this is the default value, but I want it to be explicit
	ws.max_threads(configs["max_threads"]);	// combining this with the start method, automatically implements a thread pool
	ws.connection_timeout(configs["timeout"]);
	ws.memory_limit(configs["memory_limit"]);
	ws.max_connections(configs["max_connections"]);
	ws.https_mem_key(configs["key_location"]);
	ws.https_mem_cert(configs["cert_location"]);
		
	if ((int)configs["use_ssl"] == true)
	{
		ws.use_ssl();
	}
	else
	{
		ws.no_ssl();
	}

	if ((int)configs["use_ipv6"] == true)
	{
		ws.use_ipv6();
			
		if ((int)configs["use_dual_stack"] == true)
		{
			ws.use_dual_stack();
		}
	}
	else
	{
		ws.no_ipv6();
		ws.no_dual_stack();
	}
	return ws;

}

// purpose: GET takes you to html form. PUT/POST load request contents to a file, then returns content of file
class config_resource : public file_resource
{
protected:
	std::string html_path;
	config_keeper& keeper;
public:
	config_resource(std::string i_html_path, std::string i_content_path, config_keeper& i_keeper) : html_path(i_html_path), file_resource(i_content_path, "text/plain"), keeper(i_keeper) {}

	// GET
	virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req)
	{
		srvlog.log_note(req);
		return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(html_path, 200, "text/html"));
	}


	//PUT
	virtual const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& req)
	{
		return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(content_path, 200, "text/plain"));
	}
	
	//POST

	virtual const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& req)
	{
		srvlog.log_note(req);
		std::map<std::string, config_proxy>::iterator iter;

		if (req.get_arg("reset") == "1")
		{
			keeper.ResetToDefault();	
			srvlog.log_good(req);
			return std::shared_ptr<httpserver::file_response>(new httpserver::file_response(html_path, 200, "text/html"));

		}
		//get all arguments from POST request
		const std::map<std::string, std::string, httpserver::http::arg_comparator>  argmap = req.get_args();
		//for each argument, check if its name matches a key in the config keeper's config map
		for(auto i = argmap.begin(); i != argmap.end(); i++)
		{
			iter = keeper.configs.find(i->first);
			if (iter != keeper.configs.end())
			{
				iter->second.set(i->second);
			}
		}
		keeper.WriteFile();

		if (req.content_too_large())
		{
			srvlog.log_fail(req);
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("File too large", 400));
		}

		/*std::ofstream output(content_path, std::ios::app);
		if (!output.is_open())
		{
			srvlog.log_fail(req);
			return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Error storing file", 500));
		}

		//content is NOT too large and output file opened properly
		output << req.get_content();
		output.close();
		*/
		srvlog.log_good(req);
		return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Your settings have been saved and will be applied on next server restart", 200));
	}

};

/*
httpserver::create_webserver& config_keeper::ConfigureWebserver(httpserver::create_webserver& ws)
{
	const int SMALL_CONSTANT = 2;
	int max_threads = std::thread::hardware_concurrency() * SMALL_CONSTANT;
	httpserver::http::http_utils::start_method_T start_method = httpserver::http::http_utils::INTERNAL_SELECT;
	int timeout = 180;		//default value
	int memory_limit = 32 * 1024;	//default value of 32kb
	int max_conns = FD_SETSIZE - 4;
	std::string key_location = "key.pem";
	std::string cert_location = "cert.crt";

		ws.start_method(start_method)	// this is the default value, but I want it to be explicit
		.max_threads(max_threads)	// combining this with the start method, automatically implements a thread pool
		.use_ssl()
		.connection_timeout(timeout)
		.memory_limit(memory_limit)
		.max_connections(max_conns)
		.https_mem_key(key_location)
		.https_mem_cert(cert_location)
		.use_dual_stack();

	//return ConfigureWebserver(ws, 8080);
	return ws;
}
*/

/*
.per_IP_connection_limit(int connection_limit)	//Limit on the number of(concurrent) connections made to the server from the same IP address.Can be used to prevent one IP from taking over all of the allowed connections.If the same IP tries to establish more than the specified number of connections, they will be immediately rejected.The default is 0, which means no limit on the number of connections from the same IP address.
	.bind_socket(int socket_fd)					//Listen socket to use.Pass a listen socket for the daemon to use(systemd - style).If this option is used, the daemon will not open its own listen socket(s).The argument passed must be of type "int" and refer to an existing socket that has been bound to a port and is listening.
	.max_thread_stack_size(int stack_size)		//Maximum stack size for threads created by the library.Not specifying this option or using a value of zero means using the system default (which is likely to differ based on your platform).Default is 0 (system default).
	.				//Enable or disable the IPv6 protocol support(by default, libhttpserver will just support IPv4).If you specify thisand the local platform does not support it, starting up the server will throw an exception.off by default.
	.use_dual_stack() and .no_dual_stack()		//Enable or disable the support for both IPv6and IPv4 protocols at the same time(by default, libhttpserver will just support IPv4).If you specify thisand the local platform does not support it, starting up the server will throw an exception.Note that this will mean that IPv4 addresses are returned in the IPv6 - mapped format(the ’structsockaddrin6’ format will be used for IPv4and IPv6).off by default.
	.pedantic() and .no_pedantic()		//Enables pedantic checks about the protocol(as opposed to as tolerant as possible).Specifically, at the moment, this flag causes the library to reject HTTP 1.1 connections without a Host header.This is required by the standard, but of course in violation of the “be as liberal as possible in what you accept” norm.It is recommended to turn this off if you are testing clients against the library, and on in production.off by default.
	.debug() and .no_debug()		//Enables debug messages from the library.off by default.
	.regex_checking() and .no_regex_checking()		//Enables pattern matching for endpoints.Read more here.on by default.
	.post_process() and .no_post_process()		//Enables / Disables the library to automatically parse the body of the http request as arguments if in querystring format.Read more here.on by default.
	.put_processed_data_to_content() and .no_put_processed_data_to_content()		//Enables / Disables the library to copy parsed body data to the content or to only store it in the arguments map.on by default.
	.file_upload_target(file_upload_target_T file_upload_target)		//Controls, how the library stores uploaded files.Default value is FILE_UPLOAD_MEMORY_ONLY.
			//FILE_UPLOAD_MEMORY_ONLY		//The content of the file is only stored in memory.Depending on put_processed_data_to_content only as part of the arguments map or additionally in the content.
			//FILE_UPLOAD_DISK_ONLY		//The content of the file is stored only in the file system.The path is created from file_upload_dir and either a random name(if generate_random_filename_on_upload is true) or the actually uploaded file name.
			//FILE_UPLOAD_MEMORY_AND_DISK		//The content of the file is stored in memory and on the file system.
	.file_upload_dir(const std::string & file_upload_dir)		//Specifies the directory to store all uploaded files.Default value is / tmp.
	.generate_random_filename_on_upload() and .no_generate_random_filename_on_upload()		//Enables / Disables the library to generate a unique and unused filename to store the uploaded file to.Otherwise the actually uploaded file name is used.off by default.
	.deferred() and .no_deferred()		//Enables / Disables the ability for the server to suspendand resume connections.Simply put, it enables / disables the ability to use deferred_response.Read more here.on by default.
	.single_resource() and .no_single_resource		//Sets or unsets the server in single resource mode.This limits all endpoints to be served from a single resource.The resultant is that the webserver will process the request matching to the endpoint skipping any complex semantic.Because of this, the option is incompatible with regex_checkingand requires the resource to be registered against an empty endpoint or the root endpoint("/").The resource will also have to be registered as family. (For more information on resource registration, read more here).off by default.
	Threading Models
	.start_method(const http::http_utils::start_method_T & start_method)		//libhttpserver can operate with two different threading models that can be selected through this method.Default value is INTERNAL_SELECT.
			//http::http_utils::INTERNAL_SELECT		//In this mode, libhttpserver uses only a single thread to handle listening on the portand processing of requests.This mode is preferable if spawning a thread for each connection would be costly.If the HTTP server is able to quickly produce responses without much computational overhead for each connection, this mode can be a great choice.Note that libhttpserver will still start a single thread for itself-- this way, the main program can continue with its operations after calling the start method.Naturally, if the HTTP server needs to interact with shared state in the main application, synchronization will be required.If such synchronization in code providing a response results in blocking, all HTTP server operations on all connections will stall.This mode is a bad choice if response data cannot always be provided instantly.The reason is that the code generating responses should not block(since that would block all other connections) and on the other hand, if response data is not available immediately, libhttpserver will start to busy wait on it.If you need to scale along the number of concurrent connectionand scale on multiple thread you can specify a value for max_threads(see below) thus enabling a thread pool - this is different from THREAD_PER_CONNECTION below where a new thread is spawned for each connection.
			//http::http_utils::THREAD_PER_CONNECTION		//In this mode, libhttpserver starts one thread to listen on the port for new connectionsand then spawns a new thread to handle each connection.This mode is great if the HTTP server has hardly any state that is shared between connections(no synchronization issues!) and may need to perform blocking operations(such as extensive IO or running of code) to handle an individual connection.
	.max_threads(int max_threads)		//A thread pool can be combined with the INTERNAL_SELECT mode to benefit implementations that require scalability.As said before, by default this mode only uses a single thread.When combined with the thread pool option, it is possible to handle multiple connections with multiple threads.Any value greater than one for this option will activate the use of the thread pool.In contrast to the THREAD_PER_CONNECTION mode(where each thread handles one and only one connection), threads in the pool can handle a large number of concurrent connections.Using INTERNAL_SELECT in combination with a thread pool is typically the most scalable(but also hardest to debug) mode of operation for libhttpserver.Default value is 1. This option is incompatible with THREAD_PER_CONNECTION.


	.not_found_resource(const shared_ptr<http_response>(*render_ptr)(const http_request&) resource)		//Specifies a function to handle a request when no matching registered endpoint exist for the URL requested by the client.
	.method_not_allowed_resource(const shared_ptr<http_response>(*render_ptr)(const http_request&) resource)		//Specifies a function to handle a request that is asking for a method marked as not allowed on the matching http_resource.
	.internal_error_resource(const shared_ptr<http_response>(*render_ptr)(const http_request&) resource)		//Specifies a function to handle a request that is causing an uncaught exception during its execution.REMEMBER : is this callback is causing an exception itself, the standard default response from libhttpserver will be reported to the HTTP client.

	.log_access(void(*log_access_ptr)(const std::string&) functor)		//Specifies a function used to log accesses(requests) to the server.
	.log_error(void(*log_error_ptr)(const std::string&) functor)		//Specifies a function used to log errors generating from the server.


	.use_ssl() and .no_ssl()		//Determines whether to run in HTTPS - mode or not.If you set this as on and libhttpserver was compiled without SSL support, the library will throw an exception at start of the server.off by default.
	.cred_type(const http::http_utils::cred_type_T & cred_type)		//Daemon credentials type.Either certificate or anonymous.Acceptable values are :
			//NONE: No credentials.
			//CERTIFICATE : Certificate credential.
			//ANON : Anonymous credential.
			//SRP : SRP credential.
			//PSK : PSK credential.
			//IA : IA credential.
	.https_mem_key(const std::string & filename)		//String representing the path to a file containing the private key to be used by the HTTPS daemon.This must be used in conjunction with https_mem_cert.
	.https_mem_cert(const std::string & filename)		//String representing the path to a file containing the certificate to be used by the HTTPS daemon.This must be used in conjunction with https_mem_key.
	.https_mem_trust(const std::string & filename)		//String representing the path to a file containing the CA certificate to be used by the HTTPS daemon to authenticate and trust clients certificates.The presence of this option activates the request of certificate to the client.The request to the client is marked optional, and it is the responsibility of the server to check the presence of the certificate if needed.Note that most browsers will only present a client certificate only if they have one matching the specified CA, not sending any certificate otherwise.
	.https_priorities(const std::string & priority_string)		//SSL / TLS protocol version and ciphers.Must be followed by a string specifying the SSL / TLS protocol versions and ciphers that are acceptable for the application.The string is passed unchanged to gnutls_priority_init.If this option is not specified, "NORMAL" is used.



	.ban_system() and .no_ban_system		//Can be used to enable / disable the ban system.on by default.
	.default_policy(ACCEPT)					//Specifies what should be the default behavior when receiving a request.Possible values are ACCEPTand REJECT.Default is ACCEPT.
	.basic_auth() and .no_basic_auth		//Can be used to enable / disable parsing of the basic authorization header sent by the client.on by default.
	.digest_auth() and .no_digest_auth		//Can be used to enable / disable parsing of the digested authentication data sent by the client.on by default.
	.nonce_nc_size(int nonce_size)			//Size of an array of nonceand nonce counter map.This option represents the size(number of elements) of a map of a nonceand a nonce - counter.If this option is not specified, a default value of 4 will be used(which might be too small for servers handling many requests).You should calculate the value of NC_SIZE based on the number of connections per second multiplied by your expected session duration plus a factor of about two for hash table collisions.For example, if you expect 100 digest - authenticated connections per second and the average user to stay on your site for 5 minutes, then you likely need a value of about 60000. On the other hand, if you can only expect only 10 digest - authenticated connections per second, tolerate browsers getting a fresh nonce for each requestand expect a HTTP request latency of 250 ms, then a value of about 5 should be fine.
	.digest_auth_random(const std::string & nonce_seed)		//Digest Authentication nonce’s seed.For security, you SHOULD provide a fresh random nonce when actually using Digest Authentication with libhttpserver in production.
	*/
	/*
	* .per_IP_connection_limit(int connection_limit): Limit on the number of (concurrent) connections made to the server from the same IP address. Can be used to prevent one IP from taking over all of the allowed connections. If the same IP tries to establish more than the specified number of connections, they will be immediately rejected. The default is 0, which means no limit on the number of connections from the same IP address.
.bind_socket(int socket_fd): Listen socket to use. Pass a listen socket for the daemon to use (systemd-style). If this option is used, the daemon will not open its own listen socket(s). The argument passed must be of type "int" and refer to an existing socket that has been bound to a port and is listening.
.max_thread_stack_size(int stack_size): Maximum stack size for threads created by the library. Not specifying this option or using a value of zero means using the system default (which is likely to differ based on your platform). Default is 0 (system default).
.use_ipv6() and .no_ipv6(): Enable or disable the IPv6 protocol support (by default, libhttpserver will just support IPv4). If you specify this and the local platform does not support it, starting up the server will throw an exception. off by default.
.use_dual_stack() and .no_dual_stack(): Enable or disable the support for both IPv6 and IPv4 protocols at the same time (by default, libhttpserver will just support IPv4). If you specify this and the local platform does not support it, starting up the server will throw an exception. Note that this will mean that IPv4 addresses are returned in the IPv6-mapped format (the ’structsockaddrin6’ format will be used for IPv4 and IPv6). off by default.
.pedantic() and .no_pedantic(): Enables pedantic checks about the protocol (as opposed to as tolerant as possible). Specifically, at the moment, this flag causes the library to reject HTTP 1.1 connections without a Host header. This is required by the standard, but of course in violation of the “be as liberal as possible in what you accept” norm. It is recommended to turn this off if you are testing clients against the library, and on in production. off by default.
.debug() and .no_debug(): Enables debug messages from the library. off by default.
.regex_checking() and .no_regex_checking(): Enables pattern matching for endpoints. Read more here. on by default.
.post_process() and .no_post_process(): Enables/Disables the library to automatically parse the body of the http request as arguments if in querystring format. Read more here. on by default.
.put_processed_data_to_content() and .no_put_processed_data_to_content(): Enables/Disables the library to copy parsed body data to the content or to only store it in the arguments map. on by default.
.file_upload_target(file_upload_target_T file_upload_target): Controls, how the library stores uploaded files. Default value is FILE_UPLOAD_MEMORY_ONLY.
FILE_UPLOAD_MEMORY_ONLY: The content of the file is only stored in memory. Depending on put_processed_data_to_content only as part of the arguments map or additionally in the content.
FILE_UPLOAD_DISK_ONLY: The content of the file is stored only in the file system. The path is created from file_upload_dir and either a random name (if generate_random_filename_on_upload is true) or the actually uploaded file name.
FILE_UPLOAD_MEMORY_AND_DISK: The content of the file is stored in memory and on the file system.
.file_upload_dir(const std::string& file_upload_dir): Specifies the directory to store all uploaded files. Default value is /tmp.
.generate_random_filename_on_upload() and .no_generate_random_filename_on_upload(): Enables/Disables the library to generate a unique and unused filename to store the uploaded file to. Otherwise the actually uploaded file name is used. off by default.
.deferred() and .no_deferred(): Enables/Disables the ability for the server to suspend and resume connections. Simply put, it enables/disables the ability to use deferred_response. Read more here. on by default.
.single_resource() and .no_single_resource: Sets or unsets the server in single resource mode. This limits all endpoints to be served from a single resource. The resultant is that the webserver will process the request matching to the endpoint skipping any complex semantic. Because of this, the option is incompatible with regex_checking and requires the resource to be registered against an empty endpoint or the root endpoint ("/"). The resource will also have to be registered as family. (For more information on resource registration, read more here). off by default.
Threading Models
.start_method(const http::http_utils::start_method_T& start_method): libhttpserver can operate with two different threading models that can be selected through this method. Default value is INTERNAL_SELECT.
http::http_utils::INTERNAL_SELECT: In this mode, libhttpserver uses only a single thread to handle listening on the port and processing of requests. This mode is preferable if spawning a thread for each connection would be costly. If the HTTP server is able to quickly produce responses without much computational overhead for each connection, this mode can be a great choice. Note that libhttpserver will still start a single thread for itself -- this way, the main program can continue with its operations after calling the start method. Naturally, if the HTTP server needs to interact with shared state in the main application, synchronization will be required. If such synchronization in code providing a response results in blocking, all HTTP server operations on all connections will stall. This mode is a bad choice if response data cannot always be provided instantly. The reason is that the code generating responses should not block (since that would block all other connections) and on the other hand, if response data is not available immediately, libhttpserver will start to busy wait on it. If you need to scale along the number of concurrent connection and scale on multiple thread you can specify a value for max_threads (see below) thus enabling a thread pool - this is different from THREAD_PER_CONNECTION below where a new thread is spawned for each connection.
http::http_utils::THREAD_PER_CONNECTION: In this mode, libhttpserver starts one thread to listen on the port for new connections and then spawns a new thread to handle each connection. This mode is great if the HTTP server has hardly any state that is shared between connections (no synchronization issues!) and may need to perform blocking operations (such as extensive IO or running of code) to handle an individual connection.
.max_threads(int max_threads): A thread pool can be combined with the INTERNAL_SELECT mode to benefit implementations that require scalability. As said before, by default this mode only uses a single thread. When combined with the thread pool option, it is possible to handle multiple connections with multiple threads. Any value greater than one for this option will activate the use of the thread pool. In contrast to the THREAD_PER_CONNECTION mode (where each thread handles one and only one connection), threads in the pool can handle a large number of concurrent connections. Using INTERNAL_SELECT in combination with a thread pool is typically the most scalable (but also hardest to debug) mode of operation for libhttpserver. Default value is 1. This option is incompatible with THREAD_PER_CONNECTION.


.not_found_resource(const shared_ptr<http_response>(*render_ptr)(const http_request&) resource): Specifies a function to handle a request when no matching registered endpoint exist for the URL requested by the client.
.method_not_allowed_resource(const shared_ptr<http_response>(*render_ptr)(const http_request&) resource): Specifies a function to handle a request that is asking for a method marked as not allowed on the matching http_resource.
.internal_error_resource(const shared_ptr<http_response>(*render_ptr)(const http_request&) resource): Specifies a function to handle a request that is causing an uncaught exception during its execution. REMEMBER: is this callback is causing an exception itself, the standard default response from libhttpserver will be reported to the HTTP client.

.log_access(void(*log_access_ptr)(const std::string&) functor): Specifies a function used to log accesses (requests) to the server.
.log_error(void(*log_error_ptr)(const std::string&) functor): Specifies a function used to log errors generating from the server.


.use_ssl() and .no_ssl(): Determines whether to run in HTTPS-mode or not. If you set this as on and libhttpserver was compiled without SSL support, the library will throw an exception at start of the server. off by default.
.cred_type(const http::http_utils::cred_type_T& cred_type): Daemon credentials type. Either certificate or anonymous. Acceptable values are:
NONE: No credentials.
CERTIFICATE: Certificate credential.
ANON: Anonymous credential.
SRP: SRP credential.
PSK: PSK credential.
IA: IA credential.
.https_mem_key(const std::string& filename): String representing the path to a file containing the private key to be used by the HTTPS daemon. This must be used in conjunction with https_mem_cert.
.https_mem_cert(const std::string& filename): String representing the path to a file containing the certificate to be used by the HTTPS daemon. This must be used in conjunction with https_mem_key.
.https_mem_trust(const std::string& filename): String representing the path to a file containing the CA certificate to be used by the HTTPS daemon to authenticate and trust clients certificates. The presence of this option activates the request of certificate to the client. The request to the client is marked optional, and it is the responsibility of the server to check the presence of the certificate if needed. Note that most browsers will only present a client certificate only if they have one matching the specified CA, not sending any certificate otherwise.
.https_priorities(const std::string& priority_string): SSL/TLS protocol version and ciphers. Must be followed by a string specifying the SSL/TLS protocol versions and ciphers that are acceptable for the application. The string is passed unchanged to gnutls_priority_init. If this option is not specified, "NORMAL" is used.



	.ban_system() and .no_ban_system: Can be used to enable/disable the ban system. on by default.
.default_policy(const http::http_utils::policy_T& default_policy): Specifies what should be the default behavior when receiving a request. Possible values are ACCEPT and REJECT. Default is ACCEPT.
Authentication Parameters
.basic_auth() and .no_basic_auth: Can be used to enable/disable parsing of the basic authorization header sent by the client. on by default.
.digest_auth() and .no_digest_auth: Can be used to enable/disable parsing of the digested authentication data sent by the client. on by default.
.nonce_nc_size(int nonce_size): Size of an array of nonce and nonce counter map. This option represents the size (number of elements) of a map of a nonce and a nonce-counter. If this option is not specified, a default value of 4 will be used (which might be too small for servers handling many requests). You should calculate the value of NC_SIZE based on the number of connections per second multiplied by your expected session duration plus a factor of about two for hash table collisions. For example, if you expect 100 digest-authenticated connections per second and the average user to stay on your site for 5 minutes, then you likely need a value of about 60000. On the other hand, if you can only expect only 10 digest-authenticated connections per second, tolerate browsers getting a fresh nonce for each request and expect a HTTP request latency of 250 ms, then a value of about 5 should be fine.
.digest_auth_random(const std::string& nonce_seed): Digest Authentication nonce’s seed. For security, you SHOULD provide a fresh random nonce when actually using Digest Authentication with libhttpserver in production.




	*/













#endif
