#include "../generate/loadelists.hpp"


int main()
{
	std::vector<std::unique_ptr<std::string>> namelist;
	std::string filepath = "../generate/enrichment lists/namelist.txt";
	int namecount = loadelist(namelist, filepath, 990);

	std::vector<std::unique_ptr<std::string>> cardlist;
	filepath = "../generate/enrichment lists/cardlist.txt";
	int cardcount = loadelist(cardlist, filepath, 995);

	std::vector<std::unique_ptr<std::string>> merchlist;
	filepath = "../generate/enrichment lists/merchlist.txt";
	int merchcount = loadelist(merchlist, filepath, 993);
	
	std::cout << "namecount = " << namecount << "\ncardcount = " << cardcount << "\nmerchcount = " << merchcount << std::endl;
	return 0;
}