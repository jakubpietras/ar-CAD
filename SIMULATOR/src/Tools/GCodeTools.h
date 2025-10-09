#pragma once
#include <string>
#include "Milling/CutterType.h"
#include "ARMAT.h"
#include <filesystem>

namespace fs = std::filesystem;

class GCodeTools
{
public:
	static CutterType GetCutterType(std::string filename);
	static double GetCutterSize(std::string filename);
	static std::vector<ar::mat::Vec3> LoadCoords(fs::path filepath);
	static std::vector<std::string> ReadFile(fs::path filepath);
	static std::vector<std::string> ParseLine(std::string line);
private:

};