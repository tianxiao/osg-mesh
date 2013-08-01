#pragma once
#include <functional>
#include <algorithm>
#include <vector>
class xtTestSTLFunction
{
public:
	xtTestSTLFunction(void);
	~xtTestSTLFunction(void);
};

inline void IntSquare(int &i) 
{
	i = i*i;
}

inline void PrintElement( int &i )
{
	printf("--%d\t--\n",i);
}

inline void TestFunction( std::function<void (int &)> func )
{
	std::vector<int> intlist;
	const int numint = 10;
	intlist.reserve(numint);
	for ( int i=0; i<numint; ++i ) {
		intlist.push_back(i);
	}
	std::for_each(intlist.begin(), intlist.end(), PrintElement);
	std::for_each(intlist.begin(), intlist.end(), func);
	std::for_each(intlist.begin(), intlist.end(), PrintElement);
}