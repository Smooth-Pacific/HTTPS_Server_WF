#ifndef GEN_USER_H
#define GEN_USER_H


#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <map>
#include <string>
#include <ctime>

//#define wfdebug true

bool flagcheck(std::fstream& mystream)
{
	bool retval = false;
	if (mystream.eof())
	{
		std::cerr << "eof" << std::endl;
		retval = true;
	}

	if (mystream.bad())
	{
		std::cerr << "bad" << std::endl;
		retval = true;
	}
	if (mystream.fail())
	{
		std::cerr << "fail" << std::endl;
		retval = true;
	}
	return retval;
}

bool flagcheck(std::ifstream& mystream)
{
	bool retval = false;
	if (mystream.eof())
	{
		std::cerr << "eof" << std::endl;
		retval = true;
	}

	if (mystream.bad())
	{
		std::cerr << "bad" << std::endl;
		retval = true;
	}
	if (mystream.fail())
	{
		std::cerr << "fail" << std::endl;
		retval = true;
	}
	return retval;
}

inline void readline(std::ifstream& instream, std::string& datum, std::string description)
{
	std::string refname;

	if (!getline(instream, datum))	//if cant get line
	{
		//if (instream.eof())
		//{
		instream.clear();
		instream.seekg(0, instream.beg);	//return to start of file
		if (!getline(instream, datum))		//try again
		{
#ifdef wfdebug
			flagcheck(instream);			//if it still didn't work, something's up
			std::cout << "Error in getting " << description << std::endl;
			std::cin.ignore();
#endif
			return;
		}
		//if it did work, all good
	//}
	}
	//if it did work, all good
	return;
}

//write a list of randomized names to file, first name on one line, last name on the next, and email on the third
//requires source text files with first names and source text files for last names
unsigned int gen_namelist(unsigned int namecount, std::string filepath = "../generate/enrichment lists/", std::string inlist1 = "firstname.txt", std::string inlist2 = "lastname.txt", std::string outlist1 = "namelist.txt", std::string outlist2 = "namedata.txt")
{
#ifdef wfdebug
	std::cout << "namecount = " << namecount << std::endl;
#endif
	std::string first;
	std::string last;
	std::string fullname;
	std::string email;

	unsigned int count = 0;


	//******************  Start Open Streams  ************************

	//open filestreams and verify they opened properly
	std::ifstream firstname(filepath + inlist1);
	if (!firstname.is_open())
	{
		//fail
		std::cout << "Error opening " << inlist1 << std::endl;
		return count;
	}

	std::ifstream lastname(filepath + inlist2);
	if (!lastname.is_open())
	{
		//fail
		std::cout << "Error opening " << inlist2 << std::endl;
		firstname.close();
		return count;
	}

	std::ofstream namelist(filepath + outlist1);
	if (!namelist.is_open())
	{
		//fail
		std::cout << "Error opening " << outlist1 << std::endl;
		firstname.close();
		lastname.close();
		return count;
	}

	std::ofstream datalist(filepath + outlist2);
	if (!namelist.is_open())
	{
		//fail
		std::cout << "Error opening " << outlist2 << std::endl;
		firstname.close();
		lastname.close();
		namelist.close();
		return count;
	}
	//******************  End Open Streams  ************************


	//******************  Start Randomization  ************************

	srand(time(NULL));

	datalist << "          " << std::endl;	//leading blank line of 10 "space" characters, to fill later with generated name count


	unsigned int select = rand() % 100;
	while (getline(firstname, first))
	{
		if (count == select)
		{
			break;
		}
		if (firstname.eof())
		{
			//firstname.clear();
			firstname.seekg(0, firstname.beg);
		}
		count++;
	}


	select = rand() % 100;
	count = 0;
	while (getline(lastname, last))
	{
		if (count == select)
		{
			break;
		}

		if (lastname.eof())
		{
			//lastname.clear();
			lastname.seekg(0, lastname.beg);
		}

		count++;
	}

	//******************  End Randomization  ************************


	//***********  Start Generation  *******************

	readline(firstname, first, "reference first name");
	readline(lastname, last, "reference last name");

	fullname = first + " " + last;
	//	email = first + "." + last + "@smoothceeplusplus.com";
	//	datalist << first << '\n' << last << '\n' << email << std::endl;

	std::string refname = fullname;

	//************   Start Loop	    **********************

	fullname = "";
	count = 0;

	while ((refname != fullname) && (count < namecount))
	{
		readline(firstname, first, "first name");
		readline(lastname, last, "last name");

		fullname = first + " " + last;
		email = first + "." + last + "@smoothceeplusplus.com";
		namelist << fullname << std::endl;
		datalist << fullname << '\n' << first << '\n' << last << '\n' << email << std::endl;

		count++;

#ifdef wfdebug
		std::cout << "name = " << fullname << std::endl;
#endif

	}

#ifdef wfdebug
	std::cout << "refname = " << refname << "\ncount = " << count << ", namecount = " << namecount << std::endl;
#endif
	datalist.seekp(0, datalist.beg);
	if (count / 1000000000 == 0)
	{
		datalist << count;
	}
	else
	{
		datalist << "overflow";
	}

	firstname.close();
	lastname.close();
	namelist.close();
	datalist.close();
	return count;
}

#endif