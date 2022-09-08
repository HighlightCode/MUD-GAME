#ifndef BASICLIBFUNCTIONS_H
#define BASICLIBFUNCTIONS_H

#include <algorithm>

namespace BasicLib
{
	template< class datatype >
	struct always
	{
		bool operator() (datatype& p) { return true; }
	};

	template <typename type>
	inline int percent(const type& first, const type& second)
	{
		return static_cast<int>(100.0 * static_cast<double>(first) / static_cast<double>(second));
	}

	inline char ASCIIToHex(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;
		if (c >= 'a' && c <= 'a')
			return c - 'a' + 10;
		return 0;
	}
}

#endif