//#ifndef PARSE_H
//#define PARSE_H


#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <cassert>
#include <memory>


#include "../parse/q.hpp"
//#include "../parse/parse.hpp"
#include "../query/xml_ele.hpp"	
#include "../query/marts.hpp"
#include "../query/query.hpp"
#include "../headers/time.hpp"
#include "../generate/enrich.hpp"


#define wfdebug true
/*
inline void dumpline(std::ostream& output, std::vector<std::string>& content, char separator = ' ')
{
	for (auto i = content.begin(); i != content.end(); i++)
	{
		output << *i << separator;
	}
	output << std::endl;
}
inline void dumpline(std::ostream& output, std::vector<std::string>& content, std::string separator = " ")
{
	for (auto i = content.begin(); i != content.end(); i++)
	{
		output << *i << separator;
	}
	output << std::endl;
}
*/
inline void parseline(std::shared_ptr<std::string> line, std::vector<std::string>& content)
{
	std::string fraud;
	std::string error;
	std::string transid;
	/*std::string mcc;
	std::string zip;
	std::string state;
	std::string city;
	std::string merch;
	std::string transtype;
	std::string time;
	std::string day;
	std::string month;
	std::string year;
	std::string card;
	std::string user;
	*/

	//string substr(size_t pos = 0, size_t len = npos) const;
	
	//std::size_t start = line.find_last_of(',');
	//chunk = line.substr(start);

	size_t pos = line->find_last_of(',');	//find first comma from back of string
	transid = line->substr(pos + 1);		//create a substring from that position forward
	line->erase(pos);						//erase the comma and everything from there forward

	pos = line->find_last_of(',');			//find first comma from back of string
	fraud = line->substr(pos + 1);			//create a substring from that position forward
	line->erase(pos);						//erase the comma and everything from there forward
	
	if (line->back() == '\"')
	{
		line->pop_back();	//remove the \" char from the back of the string
		line->pop_back();	//remove the comma from the back of the string
		pos = line->find_last_of('\"');
		error = line->substr(pos + 1);
		//line.erase(pos -1);	//might not need this
	}
	else
	{
		error = "";
	}

	pos = 0;
	size_t endpos;
	
	for (int i = 0; i < 13; i++)
	{
		endpos = line->find_first_of(',', pos);
		content.push_back(line->substr(pos, endpos - pos));
		pos = endpos + 1;
	}

	content.push_back(error);
	content.push_back(fraud);
	content.push_back(transid);
	
	return;
}
/*
//reads line, separates by separator (default ','), and stores each segment into the provided vector as separate elements. It two separators are found in a row, a vector element will be created with the string provided as the last parameter(default "")
inline void oldparseline(std::shared_ptr<std::string> line, std::vector<std::string>& content, char separator = ',', std::string emptyval = "")
{
	//clear any existing data in content
	content.clear();

	int lasti = 0;
	bool openquote = false;
	for (int i = 0; i < line->size(); i++)
	{
		if (!openquote)				//if quote opened, don't check for separators, only for a closing quote
		{
			if (line->at(i) == '\"')	//only dealing with double quotes for now
			{
				openquote = true;
				continue;			//ignore any separator-related stuff, proceed to next character
			}

			if (line->at(i) == separator)
			{
				if (lasti == i)		//if the previous character was also a comma
				{
					content.push_back("");
					//if(loud) std::cout << ",";
				}
				else
				{
					content.push_back(line->substr(lasti, (i - lasti)));
					//if(loud) std::cout << line.substr(lasti, (i - lasti)) << ',';
				}

				lasti = i + 1;

			}
		}	// if(openquote)
		if (line->at(i) == '\"')
		{
			openquote = false;	//close the open quote, resume normal processing at next character
			if (i > 0)
			{
				if (line->at(i-1) == separator)
				{
					line->erase(i - 1, 1);
					i--;
				}
			}
		}

	}

	//this bit might be bad, might require an empty element insertion in an else, but not important with current file
	if (line->back() != separator)
	{
		content.push_back(line->substr(lasti));
		//if(loud) std::cout << line.substr(lasti) << std::endl;
	}


	//check contents of vector
	//dumpline(std::cout, content, ' ');

}

//reads line, separates by separator (default ','), and stores each segment into the provided vector as separate elements. It two separators are found in a row, a vector element will be created with the string provided as the last parameter(default "")
inline void oldparseline(std::string& line, std::vector<std::string>& content, char separator = ',', std::string emptyval = "", bool loud = false)
{
	//clear any existing data in content
	content.clear();

	int lasti = 0;
	bool openquote = false;
	for (int i = 0; i < line.size(); i++)
	{
		if (!openquote)				//if quote opened, don't check for separators, only for a closing quote
		{
			if (line[i] == '\"')	//only dealing with double quotes for now
			{
				openquote = true;
				continue;	//ignore any separator-related stuff, proceed to next character
			}

			if (line[i] == separator)
			{
				if (lasti == i)//if the previous character was also a comma
				{
					content.push_back("");
					//if(loud) std::cout << ",";
				}
				else
				{
					content.push_back(line.substr(lasti, (i - lasti)));
					//if(loud) std::cout << line.substr(lasti, (i - lasti)) << ',';
				}

				lasti = i + 1;

			}
		}	// if(openquote)
		if (line[i] == '\"')
		{
			openquote = false;	//close the open quote, resume normal processing at next character
			if (i >= 1)
			{
				if (line[i - 1] == ',')
				{
					line.erase(i - 1, 1);
					i--;
				}
			}
		}

	}

	//this bit might be bad, might require an empty element insertion in an else, but not important with current file
	if (line.back() != separator)
	{
		content.push_back(line.substr(lasti));
		//if(loud) std::cout << line.substr(lasti) << std::endl;
	}


	//check contents of vector
	//dumpline(std::cout, content, ' ');

}
*/

//get line of text from source, make it a string, push string to queue
void LoadQueue(threadsafe_queue<std::string>& Q, bool& read_done, std::ifstream& input)
{
	std::string line;
//	int full = 0;
	int count = 0;
	while (std::getline(input, line))
	{
		if (line.back() == '\r')	//because the source file might have been windows and it might have ended lines with "\r\n" instead of just "\n"
		{
			line.pop_back();	//delete the '\r'
		}

		count++;
		line = line + "," + std::to_string(count);	//append count to be the transaction ID

		Q.push(line);
		
		//if (Q.count() > 500)
		//{
		//	std::cout << "Q full!" << std::endl;
		//	full++;
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1));	//let other threads have CPU time if Q is getting full
		//}
		//std::cout << line << std::endl;
		//std::cout << count << std::endl;			//output count to indicate current progress
	}

	read_done = true;
//	std::cout << "Reader times full: " << full << std::endl;
}

//while !read_done or !Q.empty(), grab lines from Q, parse and enrich, then load to Qpost
void ParseNQueue(threadsafe_queue<std::string>& Q, threadsafe_queue<std::vector<std::string>>& Qpost, bool& read_done, bool& parse_done, enrichvault& vault)
{
	//int count = 0;
	//int full = 0;
	//int empty = 0;
	do
	{
		while (!Q.empty())
		{
			std::vector<std::string> content;
			std::shared_ptr<std::string> line = Q.try_pop();
			if (line == nullptr)
			{
			//	std::cout << "Queue Empty!" << std::endl;
			//	empty++;
			//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue; //assuming read is not done, try again
			}


			parseline(line, content);
			vault.enrichcontent(content);

			Qpost.push(content);
			
			//if (Qpost.count() > 50)
			//{
			//	std::cout << "Qpost full!" << std::endl;
			//	full++;
			//	std::this_thread::sleep_for(std::chrono::milliseconds(100));	//let other threads have CPU time if Qpost is getting full
			//}
			//std::cout << line << std::endl;
			// 
			//std::cout << ++count << std::endl;			//output count to indicate current progress
		}

	} while (!read_done);
	
	parse_done = true;
//	std::cout << "Parser times full: " << full << std::endl;
//	std::cout << "Parser times empty: " << empty << std::endl;
}

//this function completes one query by reading the entire file. This is not great. Need to do all queries in one read, not one per read
void MakeLake(threadsafe_queue<std::vector<std::string>>& Qpost, bool& parse_done, std::ofstream& output)
{
	int count = 0;
	
	//initialize datamart objects
	auto mart1 = std::make_shared<usermart>("user list");
	auto mart2 = std::make_shared<userdatamart>("unused");
	auto mart3 = std::make_shared<zipmart>("zipcode list");
	//auto mart4 = std::make_shared<merchmart>("merchant list");
	auto mart5 = std::make_shared<statemart>("state list");
	auto mart6 = std::make_shared<citymart>("city list");
	auto mart7 = std::make_shared<insufficientmart>("insufficient balance list");
	auto mart8 = std::make_shared<typemart>("transaction type list");
	auto mart9 = std::make_shared<aftereightoveronehundredmart>("after eight and over one hundred list");

	//maybe should make a vector of ptrs to query_bots, but not gonna (yet)
	//initialize query objects
	auto query1 = std::make_shared<query_compound>("transactions after 8pm with a value over 100");
	auto query2 = std::make_shared<query_trans_types>("transaction types");
	auto query3 = std::make_shared<query_top_zips>("top five zip codes by transaction count");
	auto query4 = std::make_shared<query_top_ten>("top ten transactions by value");
	auto query5 = std::make_shared<query_user_transacts>("unique users");
	auto query6 = std::make_shared<query_insufficient>("percentage of users with at least one insufficient balance error");
	auto query7 = std::make_shared<query_insufficient_repeat>("percentage of users with more than one insufficient balance error");
	auto query8 = std::make_shared<query_bottom_online_months>("bottom five months for online transactions");
	auto query9 = std::make_shared<query_top_cities>("top 5 cities with the largest number of transactions");
	auto query10 = std::make_shared<query_fraud_rate>("percentage of transactions that were fraudulent");
	auto query11 = std::make_shared<query_merch_count>("unique merchants");
	auto query12 = std::make_shared<query_top_cities_with_online_merchants>("top 10 cities by number of resident merchants with online transactions");
	auto query13 = std::make_shared<query_top_merchants_insufficient_only>("top 5 merchants by number of insufficient balance and no other errors");
	

	do
	{
		while (!Qpost.empty())
		{
			std::vector<std::string> content;
			if (!Qpost.try_pop(content))
			{
				//std::cout << "Queue Empty!" << std::endl;
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue; //assuming read is not done, try again
			}

			
					
			
			
					//deploy datamart threads
					std::thread m1(&BuildMart, mart1, content);
					std::thread m2(&BuildMart, mart2, content);
					std::thread m3(&BuildMart, mart3, content);
					//std::thread m4(&BuildMart, mart4, content);
					std::thread m5(&BuildMart, mart5, content);
					std::thread m6(&BuildMart, mart6, content);
					std::thread m7(&BuildMart, mart7, content);
					std::thread m8(&BuildMart, mart8, content);
					std::thread m9(&BuildMart, mart9, content);
					
					//deploy query threads
					std::thread t1(&DoQuery, query1, content);	//FIX THIS: should try doing this lambda style
					std::thread t2(&DoQuery, query2, content);
					std::thread t3(&DoQuery, query3, content);
					std::thread t4(&DoQuery, query4, content);
					std::thread t5(&DoQuery, query5, content);
					std::thread t6(&DoQuery, query6, content);
					std::thread t7(&DoQuery, query7, content);
					std::thread t8(&DoQuery, query8, content);
					std::thread t9(&DoQuery, query9, content);
					std::thread t10(&DoQuery, query10, content);
					std::thread t11(&DoQuery, query11, content);
					std::thread t12(&DoQuery, query12, content);
					std::thread t13(&DoQuery, query13, content);
					

					//print the contents of the transaction to the "enriched output" file
					for (auto i = content.begin(); i != content.end(); i++)
					{
						if (i + 1 == content.end())
						{
							output << *i;	//no trailing comma
						}
						else
						{
							output << *i << ',';
						}
					}

					output << std::endl;
					
					//join datamart threads
					m1.join();
					m2.join();
					m3.join();
				//	m4.join();
					m5.join();
					m6.join();
					m7.join();
					m8.join();
					m9.join();
				
					//join query threads
					t1.join();
					t2.join();
					t3.join();
					t4.join();
					t5.join();
					t6.join();
					t7.join();
					t8.join();
					t9.join();
					t10.join();
					t11.join();
					t12.join();
					t13.join();
					
			
			count++;
			std::cout << count << std::endl;

		}
	} while (!parse_done);

	//if any datamarts needed post_process, it would happen here
	//mart1->post_process();	//this line is an example. mart1 does not need to call post_process()

	//XML constructs are complete, print reports
	
	query1->print_query();
	query2->print_query();
	query3->print_query();
	query4->print_query();
	query5->print_query();
	query6->print_query();
	query7->print_query();
	query8->print_query();
	query9->print_query();
	query10->print_query();
	query11->print_query();
	query12->print_query();
	query13->print_query();
	
}

int main()
{
	non_static_time_machine watch1;
	non_static_time_machine watch2(watch1);


	//int time_start = time_machine::Lap();

	//std::string filepath = "/home/credit_card/credit_card/";
	std::string filepath = "../tests/";
	std::string filename = "really_big_test";
	//std::string filename = "card_transaction.v1";
	//std::string infilesuffix = "";
	//std::string infilesuffix = ".txt";
	std::string infilesuffix = ".csv";
	//std::string outfilesuffix = infilesuffix;
	std::string outfilesuffix = ".txt";

	std::ifstream input(filepath + filename + infilesuffix);
	if (!input.is_open())
	{
		std::cout << "Error opening input file" << std::endl;
		return -1;
	}
	
	std::ofstream output(filepath + "out" + filename + outfilesuffix);
	if(!output.is_open())
	{
		std::cout << "Error opening output file" << std::endl;
		return -1;
	}

	enrichvault vault;
	std::string line;
	std::vector<std::string> content;
	bool read_done = false;
	bool parse_done = false;

	threadsafe_queue<std::string> Q;
	threadsafe_queue<std::vector<std::string>> Qpost;

	//Get and parse header line
	std::shared_ptr<std::string> spline = std::make_shared<std::string>("words");	//without a literal here, the compiler fails at type reconciliation
	std::getline(input, *spline);
	/*
	parseline(spline, content);
	if (!content.empty())
	{
		output << content[0];
		for (auto i = content.begin() + 1; i != content.end(); i++)
		{
			output << "," << *i;
		}
		output << std::endl;
	}
	*/

	//std::cout << "Starting Load" << std::endl;
	LoadQueue(std::ref(Q), std::ref(read_done), std::ref(input));	
	long long time_load = watch2.Lap();

	//std::cout << "Starting Parse and Enrichment" << std::endl;
	ParseNQueue(std::ref(Q), std::ref(Qpost), std::ref(read_done), std::ref(parse_done), std::ref(vault));
	long long time_parse = watch2.Lap();
	
	//std::cout << "Starting Queries and Marts" << std::endl;
	MakeLake(std::ref(Qpost), std::ref(parse_done), std::ref(output));
	long long time_query = watch2.Read();

	/*
	std::thread reader(LoadQueue, std::ref(Q), std::ref(read_done), std::ref(input));
	std::thread parser(ParseNQueue, std::ref(Q), std::ref(Qpost), std::ref(read_done), std::ref(parse_done), std::ref(vault));
	std::thread shopper(MakeLake, std::ref(Qpost), std::ref(parse_done), std::ref(output));
	
	
	reader.join();
	parser.join();
	shopper.join();
	*/

	input.close();
	output.close();

	long long time_elapsed = watch1.Read();
	std::cout << "Time elapsed:   " << time_elapsed << "ms" << std::endl;
	std::cout << "Time for Load:  " << time_load << "ms" << std::endl;
	std::cout << "Time for Parse: " << time_parse << "ms" << std::endl;
	std::cout << "Time for Query: " << time_query << "ms" << std::endl;

	return 0;
}

//#endif