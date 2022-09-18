#ifndef BASICLIBFUNCTIONS_H
#define BASICLIBFUNCTIONS_H

#include <algorithm>

namespace BasicLib
{
	template<class iterator, class function, class qualify>
	inline function operate_on_if(iterator begin,
		iterator end,
		function func,
		qualify q)
	{
		while (begin != end)
		{
			if (q(*begin))
				func(*begin);
			++begin;
		}

		return func;
	}

	template<class iterator, class firstpass, class secondpass>
	inline iterator double_find_if(iterator begin,
		iterator end,
		firstpass one,
		secondpass two)
	{
		iterator itr = std::find_if(begin, end, one);
		if (itr != end)
			return itr;

		return std::find_if(begin, end, two);
	}

	template<class iterator, class firstpass, class secondpass, class qualify>
	inline iterator double_find_if(iterator begin, iterator end, firstpass one,	secondpass two, qualify q)
	{
		iterator itr = begin;
		while (itr != end)
		{
			if (q(*itr) && one(*itr))
				return itr;
			++itr;
		}

		itr = begin;
		while (itr != end)
		{
			if (q(*itr) && two(*itr))
				return itr;
			++itr;
		}

		return itr;
	}
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