#include "GCodeTools.h"
#include "StringTools.h"
#include <fstream>
#include <iterator>

CutterType GCodeTools::GetCutterType(std::string filename)
{
	return filename[0] == 'k' ? CutterType::ROUND : CutterType::FLAT;
}

double GCodeTools::GetCutterSize(std::string filename)
{
	auto n = StringTools::LeftTrim(filename, 1);
	return std::stod(n);
}

std::vector<ar::mat::Vec3d> GCodeTools::LoadPoints(fs::path filepath)
{
	std::vector<ar::mat::Vec3d> points;
	auto lines = ReadFile(filepath);
	for (auto& line : lines)
	{
		auto tokens = ParseLine(line);
		double x = std::stod(tokens[2]) / 10.0;	// convert from mm to cm
		double y = std::stod(tokens[3]) / 10.0;
		double z = std::stod(tokens[4]) / 10.0;
		points.emplace_back(x, y, z);
	}
	return points;
}

std::vector<std::string> GCodeTools::ReadFile(fs::path filepath)
{
	std::vector<std::string> lines;
	std::ifstream infile(filepath);
	std::string line;
	while (std::getline(infile, line))
		lines.push_back(line);
	infile.close();
	return lines;
}

std::vector<std::string> GCodeTools::ParseLine(std::string line)
{
	std::vector<std::string> result;
	std::string tmp(line);
	std::array<char, 5> delims = { 'Z', 'Y', 'X', 'G', 'N'};
	for (char & delim : delims)
	{
		auto tokens = StringTools::Tokenize(tmp, delim);
		result.insert(result.begin(), tokens.back());
		tmp = tokens.front();
	}
	return result;
}
