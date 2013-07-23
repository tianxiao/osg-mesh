#pragma once
#include <sstream>

class xtConvertUtil
{
public:
	xtConvertUtil(void);
	~xtConvertUtil(void);
};


// code from
// http://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
template<typename T2, typename T1>
inline T2 lexical_cast(const T1 &in) {
    T2 out;
    std::stringstream ss;
    ss << in;
    ss >> out;
    return out;
};



