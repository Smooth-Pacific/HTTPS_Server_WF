#ifndef UTP_CONFIG_H
#define UTP_CONFIG_H

#include <iostream>
#include <fstream>




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














#endif
