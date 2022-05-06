#ifndef WF_MARTS_H
#define WF_MARTS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <map>

#include "../parse/defines.hpp"
#include "../query/xml_ele.hpp"

class mart_bot
{
public:
	//supporting overhead outside loop

	//query identifier/description
	std::string martname;
	std::string subfolder;
	//output location for mart
	std::ofstream martout;// ("reports/" + queryname + ".XML");

	//map for identifying uniqueness
	std::map<std::string, int> groupings;

	//int for tracking number of groups
	int groupcount = 0;

	//constructor
	mart_bot(std::string i_name, std::string i_subfolder = "") : martname(i_name), subfolder(i_subfolder), martout("../query/datamarts/" + i_subfolder + martname + ".mart")
	{
		if (!martout.is_open())
		{
			std::cout << "Error opening ../query/datamarts/" << subfolder << martname << ".mart\n" << martname << " will not be saved" << std::endl;
		}
	}

	virtual ~mart_bot()
	{
		martout.close();
	}

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
	//this will need a unique mart_bot derived class for each datamart with build_mart modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	virtual void build_mart(const std::vector<std::string>& content)
	{
		//do nothing
	}

	virtual void post_process()
	{
		//do nothing
	}
};

void BuildMart(std::shared_ptr<mart_bot> mart, std::vector<std::string> content)
{
	mart->build_mart(content);
}

//create a data mart with all unique user IDs and the associated name, comma separated
class usermart : public mart_bot
{
public:
	usermart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}
	//this will need a unique query_bot derived class for each actual query with run_query modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	void build_mart(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		std::string current_content = content[User];	//define the content to be used for grouping

		//"group by" check
		if (groupings[current_content] == 0)			//if this group is new
		{
			groupings[current_content]++;				//increment count 

			//create new group/entry
			martout << content[User] << "," << content[EnrichedUser] << std::endl;
		}

	}
};


//create a data mart for each unique user ID and include all data for that user
class userdatamart : public mart_bot
{
public:
	userdatamart(std::string i_name, std::string i_subfolder = "users/") : mart_bot(i_name, i_subfolder) {}

	std::vector<std::ofstream> userout;
	//map for identifying uniqueness
	std::map<std::string, std::vector<std::string>> cards;

	//int for tracking number of groups
	int groupcount = 0;

	void build_mart(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		//"group by" check
		std::string current_content = content[User];	//define the content to be used for grouping

		//"group by" check
		int mapval = groupings[current_content];		//check the number of times this group has previously been encountered
		if (mapval == 0)								//if this group is new
		{
			groupcount++;								//increment count of total groups
			groupings[current_content] = groupcount;	//assign new count value to this group
			mapval = groupcount;						//update mapval to new value for this group

			//create new group/entry
			userout.push_back(std::ofstream("../query/datamarts/" + subfolder + content[User] + ".mart"));
			if (!userout.back().is_open())
			{
				std::cout << "Error opening ../query/datamarts/" << subfolder << content[User] << ".mart\n" << content[User] << " will not be saved" << std::endl;
			}

			userout.back() << content[User] << "," << content[EnrichedUser] << std::endl;
		}

		bool new_card = true;
		for (int i = 0; i < cards[content[User]].size(); i++)
		{
			if (cards[content[User]].at(i) == content[Card])
			{
				new_card = false;
				break;
			}
		}

		if (new_card)	//if this card is new
		{
			cards[content[User]].push_back(content[Card]);	//mark card as no longer new

			//add current credit card to output
			userout.back() << content[Card] << "," << content[EnrichedCard] << std::endl;
		}

	}

};

//create a data mart with all unique zip codes
class zipmart : public mart_bot
{
public:
	zipmart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}
	//this will need a unique query_bot derived class for each actual query with run_query modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	void build_mart(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		std::string current_content = content[Zip];	//define the content to be used for grouping

		//"group by" check
		if (groupings[current_content] == 0)			//if this group is new
		{
			groupings[current_content]++;				//increment count 

			//create new group/entry
			martout << content[Zip] << std::endl;
		}

	}
};

//create a data mart with all unique merchant IDs, along with the MCC, enriched name, and enriched MCC
class merchmart : public mart_bot
{
public:
	merchmart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}
	//this will need a unique query_bot derived class for each actual query with run_query modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	void build_mart(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		std::string current_content = content[Merchant_Name];	//define the content to be used for grouping

		//"group by" check
		if (groupings[current_content] == 0)			//if this group is new
		{
			groupings[current_content]++;				//increment count 

			//create new group/entry
			martout << content[Merchant_Name] << "," << content[MCC] << "," << content[EnrichedMerchant_Name] << "," << content[EnrichedMCC] << "," << std::endl;
		}

	}
};

//create a data mart with all unique states
class statemart : public mart_bot
{
public:
	statemart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}
	//this will need a unique query_bot derived class for each actual query with run_query modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	void build_mart(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		std::string current_content = content[Merchant_State];	//define the content to be used for grouping

		//"group by" check
		if (groupings[current_content] == 0)			//if this group is new
		{
			groupings[current_content]++;				//increment count 

			//create new group/entry
			martout << content[Merchant_State] << std::endl;
		}

	}
};

//create a data mart with all unique states
class citymart : public mart_bot
{
public:
	citymart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}
	//this will need a unique query_bot derived class for each actual query with run_query modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	void build_mart(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		std::string current_content = content[Merchant_City];	//define the content to be used for grouping

		//"group by" check
		if (groupings[current_content] == 0)			//if this group is new
		{
			groupings[current_content]++;				//increment count 

			//create new group/entry
			martout << content[Merchant_City] << std::endl;
		}

	}
};


//create a data mart with all instances of insufficient balance error
class insufficientmart : public mart_bot
{
public:
	insufficientmart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}
	//this will need a unique query_bot derived class for each actual query with run_query modified to fit the appropriate conditions, at least until I have time to implement the flexible one
	void build_mart(const std::vector<std::string>& content)	//declaration without definition makes this a pure virtual class... WRONG
	{
		if (content[Errors] == "\"Insufficient Balance\"")
		{
			martout << vector_to_string(content) << std::endl;
		}
	}
};

//create a data mart with all unique transaction types
class typemart : public mart_bot
{
public:
	typemart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}

	void build_mart(const std::vector<std::string>& content)		//declaration without definition makes this a pure virtual class... WRONG
	{
		std::string current_content = content[Transaction_Type];	//define the content to be used for grouping

		//"group by" check
		if (groupings[current_content] == 0)						//if this group is new
		{
			groupings[current_content]++;							//increment count 

			//create new group/entry
			martout << content[Transaction_Type] << std::endl;
		}
	}
};

//create a data mart with all transactions occuring after 8pm with a value of more than $100
class aftereightoveronehundredmart : public mart_bot
{
public:
	aftereightoveronehundredmart(std::string i_name, std::string i_subfolder = "") : mart_bot(i_name, i_subfolder) {}

	void build_mart(const std::vector<std::string>& content)		//declaration without definition makes this a pure virtual class... WRONG
	{
		if (content[Time] >= "16:00")	//if transaction time is after 8 pm
		{
			if (std::stoi(content[Amount].substr(1)) > 100)	//discard the '$', then use stoi to get the numerical value.
			{

				//create new group/entry
				martout << vector_to_string(content) << std::endl;
			}
		}
	}
};


#endif