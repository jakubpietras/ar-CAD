#include "arpch.h"
#include "ToolPath.h"
#include "fmt/core.h"
#include <fstream>
#include <iostream>

namespace ar
{
	ToolPath::ToolPath(ar::mat::Vec3 startPoint, ToolType type)
		: m_ToolType(type), m_Length(0.f)
	{
		m_MachineCoords.push_back(startPoint);
		// test:
		AddCoordinate(startPoint + ar::mat::Vec3{ 1.0, 1.0f, 1.0f });
	}

	void ToolPath::AddCoordinate(ar::mat::Vec3 point)
	{
		const auto& lastPoint = m_MachineCoords.back();
		m_MachineCoords.push_back(point);
		m_Length += ar::mat::Length(point - lastPoint);
	}

	void ToolPath::ConvertToGCode(uint32_t order, std::filesystem::path path)
	{
		auto filename = fmt::format("{}.{}", order, ar::ToolTypeStr(m_ToolType));
		
		std::ofstream file;
		file.open(path / filename);
		if (file.is_open())
		{
			int step = 0;
			for (auto& coord : m_MachineCoords)
				file << fmt::format("N{}G01X{:.3f}Y{:.3f}Z{:.3f}\n", step++, coord.x * 10, coord.y * 10, coord.z * 10);
		}
		file.close();
	}

}