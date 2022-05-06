#include "../generate/enrichlists.hpp"

#ifndef wfdebug
#define wfdebug true
#endif

int main()
{
	generate_enrichment_data(true, 1000, 1000, 1000);
	
	return 0;
}
