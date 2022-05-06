#ifndef WF_UTOP_DEFINES_H
#define WF_UTOP_DEFINES_H


#define User 0
#define Card 1
#define Year 2
#define Month 3
#define	Day 4
#define Time 5
#define Amount 6
#define Transaction_Type 7
#define Merchant_Name 8
#define Merchant_City 9
#define Merchant_State 10
#define Zip 11
#define MCC 12
#define Errors 13
#define Is_Fraud 14
#define Transaction_ID 15
#define EnrichedUser 16
#define EnrichedCard 17
#define EnrichedMerchant_Name 18
#define EnrichedMCC 19


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
#endif