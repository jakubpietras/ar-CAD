#pragma once
#include <vector>
#include <string>

class StringTools
{
public:
	static std::vector<std::string> Tokenize(std::string str, char delimiter);
	static std::string LeftTrim(std::string str, std::string::size_type count);
};