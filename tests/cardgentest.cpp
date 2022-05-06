#include "../generate/ccard.hpp"
#include <cassert>

int test()
{
	std::string card2 = "4160 4671 1285 6595";
	std::string card1 = "4160467114278970";
	std::string card3 = "4160467114278973";

	assert(luhncheck(card1));
	assert(luhncheck(card2));
	assert(!luhncheck(card3, true));
	assert(card3.back() == '0');
	std::string sandwich = "sandwich";
	assert(!luhncheck(sandwich));
	
	return 0;
}



int main()
{
	test();

	for (int count = 0; count < 17;)
	{
		std::string card = makecard(count);
		std::cout << card << std::endl;
		assert(luhncheck(card));
		count++;
	}
	//gen_cardlist(100, "../generate/enrichment lists/cardlist.txt");
	std::cout << "card count = " << gen_cardlist(10000) << std::endl;

	return 0;
}
