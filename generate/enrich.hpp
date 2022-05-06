#ifndef ENRICH_H
#define ENRICH_H


#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <cassert>
#include <memory>


#include "../generate/loadelists.hpp"
#include "../parse/defines.hpp"


class user
{
public:
	long int index;
	//std::unordered_map<int, std::string> cards;
	std::vector<long int> cindex;
	user(const long int& i_index) :index(i_index) {}

	void add_card(const long int& i_cindex)
	{
		cindex.push_back(i_cindex);
	}

	bool check_card(const int& c_id)
	{
		return static_cast<bool>(cindex.size() > c_id);
	}

	long int get_card(const int& c_id)
	{
		if (cindex.size() > c_id)
		{
			return cindex[c_id];
		}
		return -1;
	}

};


class enrichvault
{
public:
	std::vector<std::unique_ptr<std::string>> namelist;
	std::vector<std::unique_ptr<std::string>> cardlist;
	std::vector<std::unique_ptr<std::string>> merchlist;
	std::vector<std::unique_ptr<std::string>> mcclist;

	std::unordered_map<int, std::shared_ptr<user>> usermap;
	//std::unordered_map<int, std::unordered_map<int, std::string> cardmap;
	std::unordered_map<long, unwise_int> merchmap;

	int usercount;
	int cardcount;
	int merchcount;

	int lastuser;

	std::string elist_filepath;
	//std::unordered_map<int, std::string>* lastusercards;

	enrichvault() :usercount(0), cardcount(0), merchcount(0), lastuser(-2), elist_filepath("../generate/enrichment lists/")
	{
		std::string filepath = elist_filepath + "namelist.txt";
		//int namecount = loadelist(namelist, filepath);
		loadelist(namelist, filepath, 2000);

		filepath = elist_filepath + "cardlist.txt";
		//int cardcount = loadelist(cardlist, filepath);
		loadelist(cardlist, filepath, 2000);

		filepath = elist_filepath + "merchlist.txt";
		//int merchcount = loadelist(merchlist, filepath);
		loadelist(merchlist, filepath, 2000);

		filepath = elist_filepath + "mcclist.txt";
		loadelist(mcclist, filepath, 2000);

	}

	enrichvault(std::string i_elist_filepath) :usercount(0), cardcount(0), merchcount(0), lastuser(-2), elist_filepath(i_elist_filepath)
	{
		std::string filepath = elist_filepath + "namelist.txt";
		//int namecount = loadelist(namelist, filepath);
		loadelist(namelist, filepath, 2000);

		filepath = elist_filepath + "cardlist.txt";
		//int cardcount = loadelist(cardlist, filepath);
		loadelist(cardlist, filepath, 2000);

		filepath = elist_filepath + "merchlist.txt";
		//int merchcount = loadelist(merchlist, filepath);
		loadelist(merchlist, filepath, 2000);

		filepath = elist_filepath + "mcclist.txt";
		loadelist(mcclist, filepath, 2000);

	}

	//enriches the data in supplied vector by replacing the user with a name, the credit card placeholders with semi-valid values, and replacing merchant IDs/MCCs with names
	void enrichcontent(std::vector<std::string>& content)
	{
		
		//Online transactions have blanks for state, city, and zip code. This makes it more intuitive
		if (content[Transaction_Type] == "Online Transaction")
		{
			content[Merchant_State] = "ONLINE";
			content[Merchant_City] = "ONLINE";
			content[Zip] = "ONLINE";
		}
	
		//check/generate user
		int userid = std::stoi(content[User]);		//convert userID string to int

		long int uindex;
		//if (userid != lastuser)		//not last user, load user if exists, otherwise create
		//{
		std::shared_ptr<user> spuser = usermap[userid];		//check pointer map for that id
		if (spuser == nullptr)							// if user does not already exist, create user
		{
			uindex = usercount++;
			spuser = std::make_shared<user>(uindex);
			usermap[userid] = spuser;
		}
		else
		{
			uindex = spuser->index;
		}
		//}
		//lastuser = userid;	//set lastuser for next cycle
		//**** user is now loaded as spuser ****
		//**** uindex is now available for use ****



		//check/generate cc for user
		int cardid = std::stoi(content[Card]);			//card id is NOT card index, is the index within the owning user

		long int cindex = spuser->get_card(cardid);		//returns -1 if card id is new for the user (is greater than that user's card count)

		if(cindex == -1)								//otherwise returns the index for the card's number on cardlist
		{
			cindex = cardcount++;
			spuser->add_card(cindex);
		}
		//**** card is now accessible  ****
		

		// check/generate merchant and MCC
		long int merchid = std::stol(content[MCC]);	//merchid is key to merchmap, which produces mindex
		//this is where I add a "lastmerch" check if I decide to do that 
		long int mindex = merchmap[merchid];		
		
		if (mindex == -1)			//if the merchant ID number is new, assign it an index
		{
			mindex = merchcount++;
			merchmap[merchid] = mindex;
		}
		
		//**** merch now has index ****


		//pushing updated values into content early causes issues with map calls.

		//push name into content vector to enrich ID number
		content.push_back(*(namelist[uindex]));

		//push card into content vector to enrich ID number
		content.push_back(*(cardlist[cindex]));

		//push name into content vector to enrich ID number
		content.push_back(*(merchlist[mindex]));
		content.push_back(*(mcclist[mindex]));


	}

};


#endif