#ifndef GEN_MERCH_H
#define GEN_MERCH_H
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <map>
#include <string>


#ifndef CANT_OPEN_FILE_EXCEPT
#define CANT_OPEN_FILE_EXCEPT
class file_not_open : public std::exception
{
public:
	std::string filepath;
	std::string output;
	file_not_open(std::string i_filepath = "") : filepath(i_filepath) {}
	const char* what() const throw()
	{
		if (filepath != "")
		{
			return std::move(("Error opening file: " + filepath).data());
		}

		return "Error opening file";
	}
};
#endif



#ifndef GEN_USER_H
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

#endif


//**************************************************************************************************************
//**************************************************************************************************************
//**************************************************************************************************************
//**************************************************************************************************************
//**************************************************************************************************************
//**************************************************************************************************************
//**************************************************************************************************************

//write a list of randomized names to file, first name on one line, last name on the next, and email on the third
//requires source text files with first names and source text files for last names
unsigned int gen_merchlist(unsigned int merchcount, std::string filepath = "../generate/enrichment lists/", std::string inlist1 = "lastname.txt", std::string inlist2 = "merchtype.txt", std::string inlist3 = "merchsuffix.txt", std::string outlist1 = "merchlist.txt", std::string outlist2 = "merchdata.txt", std::string outlist3 = "mcclist.txt")
{
#ifdef wfdebug
	std::cout << "merchcount = " << merchcount << std::endl;
#endif
	std::string p1;
	std::string p2;
	std::string p3;
	std::string assembled;
	std::string trunc;
	unsigned int count = 0;


	//******************  Start Open Streams  ************************

	//open filestreams and verify they opened properly
	std::ifstream part1(filepath + inlist1);
	if (!part1.is_open())
	{
		//fail
		std::cout << "Error opening " << inlist1 << std::endl;
		return count;
	}

	std::ifstream part2(filepath + inlist2);
	if (!part2.is_open())
	{
		//fail
		std::cout << "Error opening " << inlist2 << std::endl;
		part1.close();
		return count;
	}

	std::ifstream part3(filepath + inlist3);
	if (!part3.is_open())
	{
		//fail
		std::cout << "Error opening " << inlist3 << std::endl;
		part1.close();
		part1.close();
		return count;
	}
	std::ofstream list1(filepath + outlist1);
	if (!list1.is_open())
	{
		//fail
		std::cout << "Error opening " << outlist1 << std::endl;
		part1.close();
		part2.close();
		part3.close();
		return count;
	}

	std::ofstream list2(filepath + outlist2);
	if (!list2.is_open())
	{
		//fail
		std::cout << "Error opening " << outlist2 << std::endl;
		part1.close();
		part2.close();
		part3.close();
		list1.close();
		return count;
	}

	std::ofstream list3(filepath + outlist3);
	if (!list3.is_open())
	{
		//fail
		std::cout << "Error opening " << outlist3 << std::endl;
		part1.close();
		part2.close();
		part3.close();
		list1.close();
		list2.close();
		return count;
	}
	//******************  End Open Streams  ************************


	//******************  Start Randomization  ************************

	list2 << "          " << std::endl;	//leading blank line of 10 "space" characters, to fill later with generated name count

	srand(time(NULL));

	//count = 0;	//count is already 0
	unsigned int select = rand() % 100;
	while (getline(part1, p1))
	{
		if (count == select)
		{
			break;
		}
		if (part1.eof())
		{
			//firstname.clear();
			part1.seekg(0, part1.beg);
		}
		count++;
	}


	count = 0;
	select = rand() % 100;
	while (getline(part2, p2))
	{
		if (count == select)
		{
			break;
		}
		if (part2.eof())
		{
			//firstname.clear();
			part2.seekg(0, part2.beg);
		}
		count++;
	}

	count = 0;
	select = rand() % 100;
	while (getline(part3, p3))
	{
		if (count == select)
		{
			break;
		}
		if (part3.eof())
		{
			//firstname.clear();
			part3.seekg(0, part3.beg);
		}
		count++;
	}

	//******************  End Randomization  ************************


	//***********  Start Generation  *******************

	readline(part1, p1, "reference p1");
	readline(part2, p2, "reference p2");
	readline(part3, p3, "reference p3");

	assembled = p1 + "'s " + p2 + " " + p3;
	//	email = first + "." + last + "@smoothceeplusplus.com";
	//	datalist << first << '\n' << last << '\n' << email << std::endl;

	std::string reference = assembled;

	//************   Start Loop	    **********************

	assembled = "";
	count = 0;

	while ((reference != assembled) && (count < merchcount))
	{
		readline(part1, p1, "p1");
		readline(part2, p2, "p2");
		readline(part3, p3, "p3");
		assembled = p1 + "'s " + p2 + " " + p3;
		trunc = "";
		trunc.push_back(p1[0]);
		trunc.push_back(p2[0]);
		trunc.push_back(p3[0]);
		trunc.push_back(static_cast<char>(count % 10 + '0'));

		list1 << assembled << std::endl;
		list2 << assembled << std::endl;
		list2 << trunc << std::endl;
		list3 << trunc << std::endl;

		count++;

#ifdef wfdebug
		std::cout << "name = " << assembled << std::endl;
#endif

	}

#ifdef wfdebug
	std::cout << "reference = " << reference << "\ncount = " << count << ", merchcount = " << merchcount << std::endl;
#endif

	list2.seekp(0, list2.beg);
	if (count / 1000000000 == 0)
	{
		list2 << count;
	}
	else
	{
		list2 << "overflow";
	}

	part1.close();
	part2.close();
	part3.close();
	list1.close();
	list2.close();
	list3.close();
	return count;
}

#endif
