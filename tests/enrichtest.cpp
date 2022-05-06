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
#include "../query/xml_ele.hpp"	
#include "../query/marts.hpp"
#include "../query/query.hpp"
#include "../generate/enrich.hpp"


#define wfdebug true

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

//reads line, separates by separator (default ','), and stores each segment into the provided vector as separate elements. It two separators are found in a row, a vector element will be created with the string provided as the last parameter(default "")
inline void parseline(std::shared_ptr<std::string> line, std::vector<std::string>& content, char separator = ',', std::string emptyval = "")
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
inline void parseline(std::string& line, std::vector<std::string>& content, char separator = ',', std::string emptyval = "", bool loud = false)
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

//get line of text from source, make it a string, push string to queue
void LoadQueue(threadsafe_queue<std::string>& Q, bool& read_done, std::ifstream& input)
{
	std::string line;
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

		if (Q.count() > 50)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		//std::cout << line << std::endl;
		//std::cout << count << std::endl;			//output count to indicate current progress
	}

	read_done = true;
}



//this function completes one query by reading the entire file. This is not great. Need to do all queries in one read, not one per read
void Read_Big_Data(threadsafe_queue<std::string>& Q, bool& read_done, enrichvault& vault, std::ofstream& output)
{
	int count = 0;

	//initialize datamart objects
	auto mart1 = std::make_shared<usermart>("user list");
	auto mart2 = std::make_shared<userdatamart>("unused");
	auto mart3 = std::make_shared<zipmart>("zipcode list");
	auto mart4 = std::make_shared<merchmart>("merchant list");
	auto mart5 = std::make_shared<statemart>("state list");
	auto mart6 = std::make_shared<citymart>("city list");
	auto mart7 = std::make_shared<insufficientmart>("insufficient balance list");
	auto mart8 = std::make_shared<typemart>("transaction type list");
	auto mart9 = std::make_shared<aftereightoveronehundredmart>("after eight and over one hundred list");

	//maybe should make a vector of ptrs to query_bots, but not gonna (yet)
	//initialize query objects
	auto query1 = std::make_shared<query_compound>("transactions after 8pm with a value over $100");
	auto query2 = std::make_shared<query_trans_types>("transaction types");
	auto query3 = std::make_shared<query_top_zips>("top five zip codes by transaction count");
	auto query4 = std::make_shared<query_top_five>("top five transactions by value");
	auto query5 = std::make_shared<query_user_transacts>("unique users");
	auto query6 = std::make_shared<query_insufficient>("percentage of users with at least one insufficient balance error");
	auto query7 = std::make_shared<query_insufficient_repeat>("percentage of users with more than one insufficient balance error");


	do
	{
		while (!Q.empty())
		{
			std::vector<std::string> content;
			std::shared_ptr<std::string> line = Q.try_pop();
			if (line == nullptr)
			{
				std::cout << "Queue Empty!" << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue; //assuming read is not done, try again
			}

		
			//add a line ID to the end of each line
			parseline(line, content, ',');

			vault.enrichcontent(content);
			

			//deploy datamart threads
			std::thread m1(&BuildMart, mart1, content);
			std::thread m2(&BuildMart, mart2, content);
			std::thread m3(&BuildMart, mart3, content);
			std::thread m4(&BuildMart, mart4, content);
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
			m4.join();
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


			count++;
			std::cout << count << std::endl;

		}
	} while (!read_done);

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

}

int main()
{

	//std::string filepath = "/home/credit_card/credit_card/";
	std::string filepath = "../tests/";
	std::string filename = "testshort";
	//std::string filename = "card_transaction.v1";
	//std::string infilesuffix = "";
	std::string infilesuffix = ".txt";
	//std::string infilesuffix = ".csv";
	std::string outfilesuffix = infilesuffix;
	//std::string outfilesuffix = ".txt";

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

	threadsafe_queue<std::string> Q;

	//Get and parse header line
	std::getline(input, line);
	parseline(line, content);
	
	std::thread reader(LoadQueue, std::ref(Q), std::ref(read_done), std::ref(input));
	
	std::thread t1(Read_Big_Data, std::ref(Q), std::ref(read_done), std::ref(vault), std::ref(output));
	reader.join();
	
	t1.join();

	return 0;
}

//#endif