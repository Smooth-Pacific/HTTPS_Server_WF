#ifndef GEN_ENRICHLISTS_H
#define GEN_ENRICHLISTS_H

#include "../generate/name.hpp"
#include "../generate/ccard.hpp"
#include "../generate/merch.hpp"


bool CRcheck(std::string filename, std::string fileextension = ".txt")
{
	std::fstream in(filename + fileextension);
	if (!in.is_open())
	{
		//fail
		std::cerr << "Error opening " << filename << fileextension << std::endl;
		in.close();
		return false;
	}

	int negtwo = -2;

	std::string junk;
	std::getline(in, junk);
	long position = in.tellp();
	in.seekg(position + negtwo);
	if (in.get() == '\r')
	{
#ifdef wfdebug
		std::cout << "DIRTY" << std::endl;
#endif
		in.close();
		return true;
	}
#ifdef wfdebug
	flagcheck(in);
#endif
	in.clear();

	in.seekg(negtwo, in.end);
	if (in.get() == '\r')
	{
#ifdef wfdebug
		std::cout << "DIRTY" << std::endl;
#endif
		in.close();
		return true;
	}

	return false;
}


void CRpurge(std::string filename, std::string fileextension = ".txt")
{
	std::fstream in(filename + fileextension);
	if (!in.is_open())
	{
		//fail
		std::cout << "Error opening " << filename << fileextension << std::endl;
		return;
	}

	std::ofstream out("clean" + filename + fileextension);
	if (!out.is_open())
	{
		//fail
		std::cout << "Error opening clean " << filename << fileextension << std::endl;
		in.close();
		return;
	}


	if (true)
	{
		char inchar;
		while (in.good())
		{
			inchar = in.get();
			if (inchar != '\r')
			{
				//in.unget();
				if (in.good()) out.put(inchar);
			}
			else
			{
				std::cout << "\\r!" << std::endl;
			}
		}
	}

	in.close();
	out.close();

	//int rename(const char* oldname, const char* newname);
	rename((filename + fileextension).c_str(), (filename + fileextension + ".old").c_str());
	rename(("clean" + filename + fileextension).c_str(), (filename + fileextension).c_str());
}


void generate_enrichment_data(bool CRclean = false, unsigned int namecount = 100000, unsigned int cardcount = 100000, unsigned int merchcount = 100000)
{
	if (CRclean)
	{
		std::cout << "Cleaning..." << std::endl;

		std::string filename = "../generate/enrichment lists/firstname";
		if (CRcheck(filename))
		{
			CRpurge(filename);
		}

		filename = "../generate/enrichment lists/lastname";
		if (CRcheck(filename))
		{
			CRpurge(filename);
		}

		filename = "../generate/enrichment lists/merchtype";
		if (CRcheck(filename))
		{
			CRpurge(filename);
		}

		filename = "../generate/enrichment lists/merchsuffix";
		if (CRcheck(filename))
		{
			CRpurge(filename);
		}
	}

	unsigned int namemade;
	unsigned int cardmade;
	unsigned int merchmade;

	namemade = gen_namelist(namecount);
	cardmade = gen_cardlist(namecount);
	merchmade = gen_merchlist(namecount);

	std::cout << "names generated: " << namemade << std::endl;
	std::cout << "cards generated: " << cardmade << std::endl;
	std::cout << "merchants generated: " << merchmade << std::endl;

	return;
}

#endif