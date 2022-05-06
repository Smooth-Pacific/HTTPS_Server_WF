#ifndef GEN_CARD_H
#define GEN_CARD_H
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <map>
#include <string>

bool luhncheck(std::string& numbers, bool set = false)
{
	int accum = 0;
	int mult = 2;
	//std::cout << " numbers.size() = " << numbers.size() << std::endl;
	for (int i = numbers.size() - 2; i >= 0; i--)	// offset once to correct for zero-indexing and again to skip over the checksum digit
	{
		if ((numbers[i] < '0') || (numbers[i] > '9'))
		{
			if (numbers[i] == ' ')
			{
				continue;
			}
			return false;
		}
		int temp = (numbers[i] - '0') * mult;
		
		temp = temp / 10 + temp % 10;	//add the digits. temp should never be higher than 18 (digit = 9 AND mult = 2)
		accum += temp;

		if (mult == 2)
		{
			mult = 1;
		}
		else
		{
			mult = 2;
		}
	}

	accum = (10 - (accum % 10)) % 10;

	if (accum == (numbers.back() - '0'))
	{
		return true;
	}

	if (set == true)
	{
		numbers.back() = accum + '0';		
	}

	return false;

}

struct cardarch
{
	std::map<std::string, int> cardmap;
};

cardarch cardarchive;

std::string oldmakecard()
{
	int time_int = time(nullptr);
	srand(time_int);
	std::string newcard;
	for (int i = 0; i < 15; i++)
	{
		newcard.push_back(rand() % 10 + '0');
	}
	//only the first 15 digits for now, no reason to validate until it is affirmed unique

	//if this card number has already been used, will return non-zero (true)
	while (cardarchive.cardmap[newcard])
	{
		//if already used, delete and re-generate
		newcard.clear();

		for (int i = 0; i < 15; i++)
		{
			newcard.push_back(rand() % 10 + '0');
		}
	}
	
	cardarchive.cardmap[newcard]++;	//mark card as "used" on map

	newcard.push_back('0');	//this is a junk digit, will be replaced by luhncheck

	luhncheck(newcard, true);
	return newcard;
}

std::string makecard(const unsigned int count)
{
	std::string newcard;

	std::string strcount = std::to_string(count);	//convert int to string and make sure it has five numerals
	if (strcount.length() < 7)
	{
		std::string buff;
		for (int i = 0; i < 7 - strcount.length(); i++)
		{
			buff.push_back('0');
		}
		strcount = buff + strcount;
	}
	else if (strcount.length() > 7)
	{
		std::cerr << "PANIC!! credit card count too high!" << std::endl;
		return strcount;
	}


	for (int i = 0; i < 8; i++)
	{
		newcard.push_back(rand() % 10 + '0');
	}

	newcard = newcard + strcount;
	//only the first 15 digits for now, no reason to validate until it is affirmed unique

	newcard.push_back('0');	//this is a junk digit, will be replaced by luhncheck

	luhncheck(newcard, true);

	return newcard;
}

unsigned int gen_cardlist(unsigned int cardcount, std::string filename = "../generate/enrichment lists/cardlist.txt")
{
	unsigned int count = 0;
	std::ofstream cardlist(filename);
	if (!cardlist.is_open())
	{
		//fail
		std::cout << "Error opening cardlist.txt" << std::endl;
		return count;
	}
	while (count < cardcount)
	{
		cardlist << makecard(count) << std::endl;
		count++;
	}
	cardlist.close();
	return count;
}
#endif