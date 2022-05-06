#include <cassert>

#include "../generate/defines.hpp"


/*
class unwise_int
{
	int value;
public:
	unwise_int() : value(-1) {}
	unwise_int(const int& i_v) : value(i_v) {}
	int& operator ()()
	{
		return value;
	}
	operator int()
	{
		return value;
	}
	
	//pre-increment
	int& operator ++()
	{
		return ++value;
	}
	//pre-decrement
	int& operator --()
	{
		return --value;
	}
	//post-increment
	int operator ++(int)
	{
		return value++;
	}
	//post-decrement
	int operator --(int)
	{
		return value--;
	}
};
*/
int main()
{
	
	unwise_int n0;
	unwise_int n1(1);
	unwise_int n1b = 1;
	int i2 = 2;
	unwise_int n2 = i2;
	unwise_int n2b(i2);
	int i2b = n2;

	assert(n0 == -1);
	assert(n1 == 1);
	assert(n1b == 1);
	assert(n2 == 2);
	assert(n2b == 2);
	assert(i2b == 2);
	

	unwise_int negone;
	unwise_int one(1);
	int two = 2;
	int three = 3;

	int a = negone + one;
	int b = negone + two;
	int c = 2 + one + three;
	int d = negone + 4;
	//negone++;
	//one--;


	assert(a == 0);
	assert(b == 1);
	assert(c == 6);
	assert(d == 3);
	assert(++negone == 0);
	assert(one-- == 1);
	assert(--one == -1);
	assert(one == -1);
	





	return 0;
}

