#ifndef WF_QUERY_H
#define WF_QUERY_H

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <map>

#include "../parse/defines.hpp"
#include "../query/xml_ele.hpp"

class query_bot
{
public:
	//supporting overhead outside loop

	//query identifier/description
	std::string queryname;

	//output location for xml print
	std::ofstream xmlout;// ("reports/" + queryname + ".XML");

	//xml entity for holding query data
	std::shared_ptr<xml_element> queryxml;// = std::make_shared<xml_element>(queryname);	//this should maybe be a unique pointer?

	//map for identifying uniqueness
	std::map<std::string, int> groupings;

	//int for tracking number of groups
	int groupcount = 0;

	//constructor
	query_bot(std::string i_name) : queryname(i_name), xmlout("../query/reports/" + queryname + ".XML"), queryxml(std::make_shared<xml_element>(queryname))
	{
		if (!xmlout.is_open())
		{
			std::cout << "Error opening reports/" << queryname << ".XML\n" << queryname << " will not be saved" << std::endl;
			//consider throwing an exception
		}
	}

	virtual ~query_bot() {}

	// its inefficient to convert the vector back to a string rather than just pass the string by value, but I'm keeping the option open
	std::string vector_to_string(std::vector<std::string> content)
	{
		std::string output;
		for (int i = 0; i < content.size(); i++)
		{
			output += content[i];
			output += ",";
		}
		output.pop_back();
		return output;
	}
	//this will need a unique query_bot derived class for each actual query with run_query modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	virtual void run_query(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		//do nothing
	}

	//prints XML of query results. If any post-processing is needed for query, it is performed here.
	virtual void print_query()	//I kinda want to put this in the destructor, but I'm not sure about it yet
	{
		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};


void DoQuery(std::shared_ptr<query_bot> query, std::vector<std::string> content)
{
	query->run_query(content);
}

//find all the different types of transaction
class query_trans_types : public query_bot
{
public:
	query_trans_types(std::string i_name) : query_bot(i_name) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[Transaction_Type];

		//"group by" check
		int mapval = groupings[current_content];	//check the number of times this group has previously been encountered
		if (mapval == 0)							//if this group is new
		{
			//groupcount++;							//increment count of total groups	//groupcount is not needed for this query
			groupings[current_content]++;			//increment count of map for this group to mark it "not new"

			//create new group/entry
			queryxml->add_child("transaction_type", current_content);	//add a new xml element for this group
		}

		//populate existing OR new grouping
		//do nothing/skip for an existing group, as this query only wants a list of the unique types

	}
};


//for query: top 5 zip codes with the most transactions
class query_top_zips : public query_bot
{
public:
	query_top_zips(std::string i_name) : query_bot(i_name) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[Zip];		//define the content to be used for grouping						//update mapval to new value for this zip

		//online transactions show a zip code of "", but ONLINE seems like a better label
		if (current_content == "")
		{
			current_content = "ONLINE";
		}

		//"group by" check
		//int mapval = groupings[current_content];		//mapval really doesnt accomplish anything in this query	//check the number of times this group has previously been encountered
		//if (mapval == 0)								// don't do anything special if it's new, so no need to check	//if this group is new
		{
			//groupcount++;								//groupcount is not needed for this query	//increment count of total groups

			//create new group/entry
			//queryxml->add_child(current_content);		//more practical not to populate xml entity yet	//add a new xml element for this group
		}

		//populate existing OR new grouping
		//mapval++;										//mapval really doesnt accomplish anything in this query	//increment the count for this group to track the number of occurrences
		groupings[current_content]++;					//increment count in map
		//queryxml->children[mapval - 1].text = mapval;	//more practical not to populate xml entity yet	//update text of xml element to new number of instances

	}

	//prints XML of query results. If any post-processing is needed for query, it is performed here.
	void print_query()
	{
		//post-processing - find out which 5 zip codes had the most transactions. delete all other zip codes

		//create a vector to store top 5
		std::vector<std::pair<std::string, int>> results;

		for (auto i = groupings.begin(); i != groupings.end(); i++)
		{
			bool not_inserted = true;
			for (auto j = results.begin(); j != results.end(); j++)	//might be able to make a faster algorithm with rbegin(), but maybe not
			{
				if (i->second > j->second)	//if the count for the current map element is greater than the count in element j of results
				{
					results.insert(j, { i->first, i->second });	//insert BEFORE element j
					not_inserted = false;
					break;	//exit inner loop and begin checking next element of groupings
				}
			}
			if (not_inserted)
			{
				if (results.size() < 5)
				{
					results.push_back({ i->first, i->second });
				}
			}

			while (results.size() > 5)	//should never need to pop_back more than once per iteration of the for loop, but accomodating for it is low-cost
			{
				results.pop_back();
			}
		}

		//use the results vector to build xml entity
		for (auto i = results.begin(); i != results.end(); i++)
		{
			queryxml->add_child(i->first, std::to_string(i->second));	//this creates the element with the zip code as the tag and the count as content. count was not required, but seemed good
		}

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};

//for query: top ten transactions (by value)
class query_top_ten : public query_bot
{
public:
	query_top_ten(std::string i_name) : query_bot(i_name) {}

	//create a vector to store top 10
	std::vector<std::pair<std::string, int>> results;

	void run_query(const std::vector<std::string>& content)
	{
		std::string current_content = content[Amount];					//define the content to be used for grouping
		int current_value = std::stoi(content[Amount].substr(1));		//discard the '$', then use stoi to get the numerical value.

		bool not_inserted = true;
		for (auto j = results.begin(); j != results.end(); j++)			//might be able to make a faster algorithm with rbegin(), but maybe not
		{
			if (current_value > j->second)								//if the count for the current map element is greater than the count in element j of results
			{
				//add the contents of the current transaction to the vector
				std::string content_as_string = vector_to_string(content);
				results.insert(j, { content_as_string, current_value });	//insert BEFORE element j
				not_inserted = false;
				break;													//exit inner loop and begin checking next element of groupings
			}
		}

		//add new entry to the end of the vector if it has not already been inserted AND it is not yet full 
		if (not_inserted)
		{
			if (results.size() < 10)
			{
				//add the contents of the current transaction to the vector
				std::string content_as_string = vector_to_string(content);
				results.push_back({ content_as_string, current_value });
			}
		}

		while (results.size() > 10)	//should never need to pop_back more than once per iteration of the for loop, but accomodating for it is low-cost
		{
			results.pop_back();
		}

	}

	void print_query()
	{
		//post-processing  - populate xml entity with data from results vector

		for (auto i = results.begin(); i != results.end(); i++)
		{
			queryxml->add_child("$" + std::to_string(i->second), i->first);	//this creates the element with the amount as the tag and the transaction as content
		}

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};

//for query: all deposits/payments per user
class query_user_transacts : public query_bot
{
public:

	//supporting overhead data
	std::vector<std::pair<std::string, std::ofstream>> xml_vec;

	query_user_transacts(std::string i_name) : query_bot(i_name) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[User];	//define the content to be used for grouping

		//"group by" check
		int mapval = groupings[current_content];		//check the number of times this group has previously been encountered
		if (mapval == 0)								//if this group is new
		{
			xml_vec.push_back({ content[User], std::ofstream("users/" + current_content + ".XML") });	//create a vector element with a new output stream for the new user
			if (!xml_vec.back().second.is_open())
			{
				std::cout << "Error opening " << current_content << "\n" << current_content << " will not be saved" << std::endl;
			}
			xml_vec.back().second << "<user>" << content[User] << std::endl;	//create top-level xml tag

			groupcount++;								//increment count of total groups
			groupings[current_content] = groupcount;	//assign new count value to this group
			mapval = groupcount;						//update mapval to new value for this group

			//create new group/entry
			queryxml->add_child("user", current_content);		//add a new xml element for this group
		}

		//populate existing OR new grouping

		// have to write the file one line at a time, as I cannot reasonably hold literally all the data before outputting
		std::string content_as_string = vector_to_string(content);
		xml_vec[mapval - 1].second << "\t<transaction>" << content_as_string << "</transaction>" << std::endl;	//add current transaction to the appropriate output stream

	}

	void print_query()
	{
		//post-processing 


		//XML construct is complete, print report

		//print list of users to reports folder
		queryxml->print(xmlout);
		xmlout.close();

		//add final line to each output file and close stream
		for (auto i = xml_vec.begin(); i != xml_vec.end(); i++)
		{
			i->second << "</user>" << i->first;	//close top-level xml tag
			i->second.close();
		}
	}
};

//for query:insufficient balance - percentage of users with at least one "insufficient balance" error
class query_insufficient : public query_bot
{
public:

	//map for identifying uniqueness
	std::map<std::string, int> errorsgroups;

	int erroruser_count;

	query_insufficient(std::string i_name) : query_bot(i_name), erroruser_count(0) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[User];	//define the content to be used for grouping

		//"group by" check
		int mapval = groupings[current_content];		//check the number of times this group has previously been encountered
		if (mapval == 0)								//if this group is new
		{
			groupcount++;								//increment count of total groups
			groupings[current_content] = groupcount;	//assign new count value to this group
		}

		if (content[Errors] == "\"Insufficient Balance\"")		//if transaction has insufficient balance error
		{
			mapval = errorsgroups[current_content];				//check the number of times this group has previously been encountered
			if (mapval == 0)									//if this group is new
			{
				erroruser_count++;								//increment count of total groups
				errorsgroups[current_content] = erroruser_count;	//assign new count value to this group
			}
		}
	}

	void print_query()
	{
		//post-processing 
		float error_percent = static_cast<float>(erroruser_count) / static_cast<float>(groupcount) * 100;
		queryxml->add_child("percentage", std::to_string(error_percent));		//add a new xml element for this group

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};

//for query:repeated insufficient balance - percentage of users with more than one "insufficient balance" error
class query_insufficient_repeat : public query_bot
{
public:

	//map for identifying uniqueness
	std::map<std::string, int> errorsgroups;

	int erroruser_count;

	query_insufficient_repeat(std::string i_name) : query_bot(i_name), erroruser_count(0) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[User];	//define the content to be used for grouping

		//"group by" check
		int mapval = groupings[current_content];		//check the number of times this group has previously been encountered
		if (mapval == 0)								//if this group is new
		{
			groupcount++;								//increment count of total groups
			groupings[current_content] = groupcount;	//assign new count value to this group
		}

		if (content[Errors] == "\"Insufficient Balance\"")		//if transaction has insufficient balance error
		{
			errorsgroups[current_content]++;					//assign new count value to this group

			mapval = errorsgroups[current_content];				//check the number of times this group has previously been encountered
			if (mapval == 2)									//if this group is new
			{
				erroruser_count++;
			}
		}
	}

	void print_query()
	{
		//post-processing 
		float error_percent = static_cast<float>(erroruser_count) / static_cast<float>(groupcount) * 100;
		queryxml->add_child("percentage", std::to_string(error_percent));		//add a new xml element for this group

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};

//for query: transactions after 8pm with dollar value over 100, grouped by zip code
class query_compound : public query_bot
{
public:
	query_compound(std::string i_name) : query_bot(i_name) {}

	void run_query(const std::vector<std::string>& content)
	{
		if (content[Time] >= "16:00")	//if transaction time is after 8 pm
		{
			//additional limiting if(s) as needed
			if (std::stoi(content[Amount].substr(1)) > 100)	//discard the '$', then use stoi to get the numerical value.
			{
				//"group by" map
				std::string current_content = content[Zip];	//define the content to be used for grouping

				//online transactions show a zip code of "", but ONLINE seems like a better label
				if (current_content == "")
				{
					current_content = "ONLINE";
				}

				//"group by" check
				int mapval = groupings[current_content];		//check the number of times this group has previously been encountered
				if (mapval == 0)								//if this group is new
				{
					groupcount++;								//increment count of total groups
					groupings[current_content] = groupcount;	//assign new count value to this group
					mapval = groupcount;						//update mapval to new value for this group

					//create new group/entry
					queryxml->add_child(current_content);		//add a new xml element for this group
				}

				//populate existing OR new grouping
				//add the contents of the current transaction to a new transaction line of the XML
				std::string content_as_string = vector_to_string(content);
				queryxml->children[mapval - 1].add_child("transaction", content_as_string);

				//show on console that a new zip code was found
				//std::cout << "zip: " << current_content << ", mapval: " << mapval << std::endl;
			}
		}
	}
};

//for query: bottom 5 months with the fewest online transactions
class query_bottom_online_months : public query_bot
{
public:
	query_bottom_online_months(std::string i_name) : query_bot(i_name) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[Month];		//define the content to be used for grouping						//update mapval to new value for this zip

		//"group by" check
		//int mapval = groupings[current_content];		//mapval really doesnt accomplish anything in this query	//check the number of times this group has previously been encountered
		if (content[Zip] == "ONLINE")								// don't do anything special if it's new, so no need to check	//if this group is new
		{
			//groupcount++;								//groupcount is not needed for this query	//increment count of total groups
			groupings[current_content]++;					//increment count in map

			//create new group/entry
			//queryxml->add_child(current_content);		//more practical not to populate xml entity yet	//add a new xml element for this group
		}

		//populate existing OR new grouping
		//mapval++;										//mapval really doesnt accomplish anything in this query	//increment the count for this group to track the number of occurrences
		//queryxml->children[mapval - 1].text = mapval;	//more practical not to populate xml entity yet	//update text of xml element to new number of instances

	}

	//prints XML of query results. If any post-processing is needed for query, it is performed here.
	void print_query()
	{
		//post-processing - find out which 5 months had the fewest online transactions. delete all other zip codes

		//create a vector to store bottom 5
		std::vector<std::pair<std::string, int>> results;

		for (auto i = groupings.begin(); i != groupings.end(); i++)	//for each month
		{
			bool not_inserted = true;
			for (auto j = results.begin(); j != results.end(); j++)	//compare the month's value against the elements in the results vector	//might be able to make a faster algorithm with rbegin(), but maybe not
			{
				if (i->second < j->second)	//if the count for the current map element is less than the count in element j of results
				{
					results.insert(j, { i->first, i->second });	//insert BEFORE element j
					not_inserted = false;
					break;	//exit inner loop and begin checking next element of groupings
				}
			}
			if (not_inserted)
			{
				if (results.size() < 5)
				{
					results.push_back({ i->first, i->second });
				}
			}

			while (results.size() > 5)	//should never need to pop_back more than once per iteration of the for loop, but accomodating for it is low-cost
			{
				results.pop_back();
			}
		}

		//use the results vector to build xml entity
		for (auto i = results.begin(); i != results.end(); i++)
		{
			queryxml->add_child(i->first, std::to_string(i->second));	//this creates the element with the zip code as the tag and the count as content. count was not required, but seemed good
		}

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};


//for query: top 5 cities with the most transactions
class query_top_cities : public query_bot
{
public:
	query_top_cities(std::string i_name) : query_bot(i_name) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[Merchant_City];		//define the content to be used for grouping						//update mapval to new value for this zip

		groupings[current_content]++;					//increment count in map
	}

	//prints XML of query results. If any post-processing is needed for query, it is performed here.
	void print_query()
	{
		//post-processing - find out which 5 cities had the most transactions. delete all other cities

		//create a vector to store top 5
		std::vector<std::pair<std::string, int>> results;

		for (auto i = groupings.begin(); i != groupings.end(); i++)
		{
			bool not_inserted = true;
			for (auto j = results.begin(); j != results.end(); j++)	//might be able to make a faster algorithm with rbegin(), but maybe not
			{
				if (i->second > j->second)	//if the count for the current map element is greater than the count in element j of results
				{
					results.insert(j, { i->first, i->second });	//insert BEFORE element j
					not_inserted = false;
					break;	//exit inner loop and begin checking next element of groupings
				}
			}
			if (not_inserted)
			{
				if (results.size() < 5)
				{
					results.push_back({ i->first, i->second });
				}
			}

			while (results.size() > 5)	//should never need to pop_back more than once per iteration of the for loop, but accomodating for it is low-cost
			{
				results.pop_back();
			}
		}

		//use the results vector to build xml entity
		for (auto i = results.begin(); i != results.end(); i++)
		{
			queryxml->add_child(i->first, std::to_string(i->second));	//this creates the element with the city as the tag and the count as content. count was not required, but seemed good
		}

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};

//for query: percentage of total transactions that were marked as fraud
class query_fraud_rate : public query_bot
{
public:

	//map for identifying uniqueness
	std::map<std::string, int> errorsgroups;

	int trans_count;
	int fraud_count;

	query_fraud_rate(std::string i_name) : query_bot(i_name), trans_count(0), fraud_count(0) {}

	void run_query(const std::vector<std::string>& content)
	{
		if (content[Is_Fraud] != "No")
		{
			fraud_count++;						//increment count of fraud cases group
		}
		trans_count++;
	}

	void print_query()
	{
		//post-processing 
		float fraud_percent = static_cast<float>(fraud_count) / static_cast<float>(trans_count) * 100;
		queryxml->add_child("fraudulent transactions", std::to_string(fraud_count));		//add a new xml element for this group
		queryxml->add_child("total transactions", std::to_string(trans_count));		//add a new xml element for this group
		queryxml->add_child("percentage", std::to_string(fraud_percent));		//add a new xml element for this group

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};


//create a list of all the unique merchants
class query_unique_merch : public query_bot
{
public:
	query_unique_merch(std::string i_name) : query_bot(i_name) 
	{
		queryxml->add_child("merchant_count");	//add a new xml element for this group
	}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[EnrichedMerchant_Name];

		//"group by" check
		int mapval = groupings[current_content];	//check the number of times this group has previously been encountered
		if (mapval == 0)							//if this group is new
		{
			//groupcount++;							//increment count of total groups	//groupcount is not needed for this query
			groupings[current_content]++;			//increment count of map for this group to mark it "not new"

			//create new group/entry
			queryxml->add_child("merchant_name", current_content, "ID = " + content[Merchant_Name]);	//add a new xml element for this group
		}
	}

	void print_query()
	{
		//post-processing 
		queryxml->children[0].text = groupings.size();	//populate the preface element with the count

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};


//create a count of all the unique merchants
class query_merch_count : public query_bot
{
public:
	int count;
	query_merch_count(std::string i_name) : query_bot(i_name), count(0) {}

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[EnrichedMerchant_Name];

		//"group by" check
		int mapval = groupings[current_content];	//check the number of times this group has previously been encountered
		if (mapval == 0)							//if this group is new
		{
			//groupcount++;							//increment count of total groups	//groupcount is not needed for this query
			groupings[current_content]++;			//increment count of map for this group to mark it "not new"
			count++;
			//create new group/entry
		}

	}

	void print_query()
	{
		//post-processing 
		queryxml->add_child("Unique Merchant Count", std::to_string(count));		//add a new xml element for this group

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};

//for query: top 10 cities by number of merchants that have at least one online transaction
class query_top_cities_with_online_merchants : public query_bot
{
public:
	query_top_cities_with_online_merchants(std::string i_name) : query_bot(i_name) {}
	std::string extra_content;

	std::map<std::string, int> cities;

	//map for identifying onlineness
	std::map<std::string, std::map<std::string, int>> merchs_in_city;

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[Merchant_City];		//define the content to be used for grouping						//update mapval to new value for this zip
		if (current_content == "ONLINE")	//if the current transaction is online
		{
			if (groupings[content[EnrichedMerchant_Name]] == 0)	//check if this merchant name has online transactions yet
			{
				groupings[content[EnrichedMerchant_Name]]++;	//mark it as an online merchant
			}
		}
		else
		{
			extra_content = content[EnrichedMerchant_Name];
			//if current transaction is NOT online
			if (cities[current_content] == 0)						//if current city is new
			{
				cities[current_content]++;
				merchs_in_city[current_content][extra_content]++;	//if the city is new, the merch is new to the city
			}
			else	//if current city is NOT new
			{
				if (merchs_in_city[current_content][extra_content] == 0)	//if this merchant is new in this city
				{
					merchs_in_city[current_content][extra_content]++;		//mark the merchant as present in the city
				}
			}
		}
	}

	//prints XML of query results. If any post-processing is needed for query, it is performed here.
	void print_query()
	{
		//post-processing - find out which 5 cities had the most transactions. delete all other cities
		 
		//use the three maps to create a single map of type {city, resident online merchants} 
		//get a city that exists
		for (auto i = cities.begin(); i != cities.end(); i++)	
		{
			int online_merch_count = 0;
			//get a list of merchants that were found in that city and check each merchant in the list to see if they are online
			for (auto j = merchs_in_city[i->first].begin(); j != merchs_in_city[i->first].end(); j++)	
			{
				//j.first is merch, j.second is exists. Should all be 1 at this point

				if (groupings[j->first])	//then this merchant is online
				{
					online_merch_count++;
				}
			}

			//record the number of online mechants to the city
			i->second = online_merch_count;
			//proceed to check next city
		}

		//rank the top 10 cities
		//create a vector to store top 10
		std::vector<std::pair<std::string, int>> results;

		for (auto i = cities.begin(); i != cities.end(); i++)
		{
			bool not_inserted = true;
			for (auto j = results.begin(); j != results.end(); j++)	//might be able to make a faster algorithm with rbegin(), but maybe not
			{
				if (i->second > j->second)	//if the count for the current map element is greater than the count in element j of results
				{
					results.insert(j, { i->first, i->second });	//insert BEFORE element j
					not_inserted = false;
					break;	//exit inner loop and begin checking next element of groupings
				}
			}
			if (not_inserted)
			{
				if (results.size() < 10)
				{
					results.push_back({ i->first, i->second });
				}
			}

			while (results.size() > 10)	//should never need to pop_back more than once per iteration of the for loop, but accomodating for it is low-cost
			{
				results.pop_back();
			}
		}

		//use the results vector to build xml entity
		for (auto i = results.begin(); i != results.end(); i++)
		{
			queryxml->add_child(i->first, std::to_string(i->second));	//this creates the element with the city as the tag and the count as content. count was not required, but seemed good
		}

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};



//for query: top 5 merchants by number of insufficient balance errors while having no other errors
class query_top_merchants_insufficient_only : public query_bot
{
public:
	query_top_merchants_insufficient_only(std::string i_name) : query_bot(i_name) {}
	
	std::map<std::string, int> disqualified;

	void run_query(const std::vector<std::string>& content)
	{
		//"group by" map
		std::string current_content = content[EnrichedMerchant_Name];
		std::string extra_content = content[Errors];					//define the content to be used for grouping						//update mapval to new value for this zip
		if (extra_content == "")										//if the current transaction is has no error
		{
			return;	//this transaction is not important for this query
		}
		
		if (extra_content == "Insufficient Balance")					//if the current transaction's error is insufficient balance
		{
			groupings[content[EnrichedMerchant_Name]]++;				//increment the insufficient balance count
		}
		else
		{
			disqualified[current_content] = 1;							//doesn't matter what this was before, either adding correct new information, or re-affirming existing correct information
		}
	}

	//prints XML of query results. If any post-processing is needed for query, it is performed here.
	void print_query()
	{
		//post-processing - find out which 5 cities had the most transactions. delete all other cities

		//rank the top 5 (non-disqualified) merchants
		//create a vector to store top 5
		std::vector<std::pair<std::string, int>> results;

		for (auto i = groupings.begin(); i != groupings.end(); i++)
		{
			if (disqualified[i->first] == 1)
			{
				continue;	//this merchant is disqualified, skip to next merchant
			}

			bool not_inserted = true;
			for (auto j = results.begin(); j != results.end(); j++)	//might be able to make a faster algorithm with rbegin(), but maybe not
			{
				if (i->second > j->second)	//if the count for the current map element is greater than the count in element j of results
				{
					results.insert(j, { i->first, i->second });	//insert BEFORE element j
					not_inserted = false;
					break;	//exit inner loop and begin checking next element of groupings
				}
			}
			if (not_inserted)
			{
				if (results.size() < 5)
				{
					results.push_back({ i->first, i->second });
				}
			}

			while (results.size() > 5)	//should never need to pop_back more than once per iteration of the for loop, but accomodating for it is low-cost
			{
				results.pop_back();
			}
		}

		//use the results vector to build xml entity
		for (auto i = results.begin(); i != results.end(); i++)
		{
			queryxml->add_child(i->first, std::to_string(i->second));	//this creates the element with the merchant as the tag and the count as content. count was not required, but seemed good
		}

		//XML construct is complete, print report
		queryxml->print(xmlout);
		xmlout.close();
	}
};
#endif