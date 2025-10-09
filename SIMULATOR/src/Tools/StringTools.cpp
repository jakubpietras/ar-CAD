#include "StringTools.h"

std::vector<std::string> StringTools::Tokenize(std::string str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string::size_type pos = 0;
	std::string s(str);
	do 
	{
		pos = s.find(delimiter);
		if (pos != std::string::npos)
		{
			tokens.push_back(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else
			tokens.push_back(s);
	} 
	while (pos != std::string::npos);
	return tokens;
}

std::string StringTools::LeftTrim(std::string str, std::string::size_type count)
{
	if (str.size() < count || str.empty())
		return str;
	return str.substr(count);
}

