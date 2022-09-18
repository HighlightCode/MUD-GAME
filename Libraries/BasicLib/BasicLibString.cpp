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
		std::string str = p_string;
		str.erase(str.begin(), std::find_if(str.begin(), str.end(),
			[](unsigned char ch) {return !std::isspace(ch); }
		));
		return str;
	}

	std::string ParseWord(const std::string& p_string, int p_index)
	{
		int wss = p_string.find_first_not_of(WHITESPACE);

		while (p_index > 0)
		{
			p_index--;

			wss = p_string.find_first_of(WHITESPACE, wss);

			wss = p_string.find_first_not_of(WHITESPACE, wss);
		}

		// find the end of the word
		int wse = p_string.find_first_of(WHITESPACE, wss);

		if (wss == std::string::npos)
		{
			wss = 0;
			wse = 0;
		}

		return p_string.substr(wss, static_cast<size_t>(wse) - wss);
	}

	std::string RemoveWord(const std::string& p_string, int p_index)
	{
		int wss = p_string.find_first_not_of(WHITESPACE);

		while (p_index > 0)
		{
			p_index--;

			wss = p_string.find_first_of(WHITESPACE, wss);

			wss = p_string.find_first_not_of(WHITESPACE, wss);
		}

		// find the end of the word
		int wse = p_string.find_first_of(WHITESPACE, wss);

		// find the beginning of the next word
		wse = p_string.find_first_not_of(WHITESPACE, wse);

		if (wss == std::string::npos)
		{
			wss = 0;
			wse = 0;
		}

		std::string str = p_string;
		str.erase(wss, static_cast<size_t>(wse) - wss);
		return str;
	}


}