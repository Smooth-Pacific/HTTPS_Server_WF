#include "../generate/name.hpp"

#ifndef wfdebug
#define wfdebug true
#endif

int main(int argc, const char * argv[])
{
	if (argc > 2)
	{
		if (argv[2][0] == 'y')
		{
			std::cout << "Cleaning..." << std::endl;

			std::string filename = "firstname";
			if (CRcheck(filename))
			{
				CRpurge(filename);
			}

			filename = "lastname";
			if (CRcheck(filename))
			{
				CRpurge(filename);
			}
		}
	}

	unsigned int count;
	if(argc > 1)  
	{
		count = gen_namelist(atoi(argv[1]));
	} 
	else
	{
		std::cout << "else invoked" << std::endl;
		count = gen_namelist(10000);
	}

	std::cout << "names generated: " << count << std::endl;

	return 0;
}

