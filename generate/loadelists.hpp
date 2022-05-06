#ifndef LOADELIST_H
#define LOADELIST_H


#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <cassert>
#include <memory>


#include "../generate/name.hpp"
#include "../generate/merch.hpp"
#include "../generate/ccard.hpp"
#include "../parse/q.hpp"
#include "../query/xml_ele.hpp"	
//#include "../generate/marts.hpp"
//#include "../generate/query.hpp"

//#include "../generate/loadelists.hpp"


int loadelist(std::vector<std::unique_ptr<std::string>>& list, const std::string& filepath, int max_elements = 5000, int skips = 0)
{
	list.reserve(max_elements);
	int count = 0;
	std::unique_ptr<std::string> enrich;
	std::ifstream liststream(filepath);
	if (!liststream.is_open())
	{
		std::cerr << "Cannot open " << filepath << std::endl;
		return count;
	}
	enrich = std::make_unique<std::string>();

	while (skips > 0)
	{
		getline(liststream, *enrich);
		skips--;
	}

	while (getline(liststream, *enrich)&&(count < max_elements))
	{
		list.push_back(std::move(enrich));
		enrich = std::make_unique<std::string>();
		count++;
	}


#ifdef wfdebug
	//for (int i = 0; i < 10; i++)
	for (auto i = list.begin(); i != list.end(); i++)
	{
		//std::cout << *list.at(i) << std::endl
		std::cout << **i << std::endl;
	}
#endif

	liststream.close();
	//list.shrink_to_fit();	//removed as it can cause reallocation, which burns time I'm trying to save
	return count;
}

//there's basically no reason to ever use this function as written, but it's a good copy-paste reference
int loadallelists()
{
	std::vector<std::unique_ptr<std::string>> namelist;
	std::string filepath = "../generate/enrichment lists/namelist.txt";
	int namecount = loadelist(namelist, filepath, 10);

	std::vector<std::unique_ptr<std::string>> cardlist;
	filepath = "../generate/enrichment lists/cardlist.txt";
	int cardcount = loadelist(cardlist, filepath, 5, 3);

	std::vector<std::unique_ptr<std::string>> merchlist;
	filepath = "../generate/enrichment lists/merchlist.txt";
	int merchcount = loadelist(merchlist, filepath, 7, 2);

	std::vector<std::unique_ptr<std::string>> mcclist;
	filepath = "../generate/enrichment lists/mcclist.txt";
	loadelist(mcclist, filepath, 6, 2);

	return namecount + cardcount + merchcount;
}

#endif