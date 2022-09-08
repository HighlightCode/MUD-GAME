#include "BasicLibString.h"
#include <cctype>
#include <stdlib.h>
#include <algorithm>

namespace BasicLib
{
	std::string UpperCase(const std::string& p_string)
	{
		std::string r_string = p_string;
		std::transform(r_string.begin(), r_string.end(), r_string.begin(), ::toupper);
		return r_string;
	}

	std::string LowerCase(const std::string& p_string)
	{
		std::string r_string = p_string;
		std::transform(r_string.begin(), r_string.end(), r_string.begin(), ::tolower);
		return r_string;
	}

	std::string SearchAndReplace(const std::string& p_target, const std::string& p_search, const std::string& p_replace)
	{
		std::string str(p_target);
		std::string::size_type i = str.find(p_search);

		// loop while replacing all occurances
		while (i != std::string::npos)
		{
			str.replace(i, p_search.size(), p_replace);
			i = str.find(p_search, i + 1);
		}

		return str;
	}

	std::string TrimWhiteSpace(const std::string& p_string)
	{
		return std::string();
	}

	std::string ParseWord(const std::string& p_string, int p_index)
	{
		return std::string();
	}

	std::string RemoveWord(const std::string& p_string, int p_index)
	{
		return std::string();
	}


}