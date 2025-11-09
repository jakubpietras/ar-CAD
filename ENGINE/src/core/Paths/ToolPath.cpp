#include "arpch.h"
#include "ToolPath.h"
#include "fmt/core.h"
#include <fstream>
#include <iostream>

namespace ar
{
	ToolPath::ToolPath(ar::mat::Vec3 startPoint, ToolType type, float baseHeight)
		: m_ToolType(type), m_Length(0.f), m_BaseDisplacement(ar::mat::Vec3{0.0f, 0.0f, baseHeight})
	{
		m_MachineCoords.push_back(startPoint + m_BaseDisplacement);
	}

	void ToolPath::MoveTo(ar::mat::Vec3 point)
	{
		const auto& lastPoint = m_MachineCoords.back() - m_BaseDisplacement;
		m_MachineCoords.push_back(point + m_BaseDisplacement);
		m_Length += ar::mat::Length(point - lastPoint);
	}

	void ToolPath::MoveBy(ar::mat::Vec3 v)
	{
		const auto& lastPoint = m_MachineCoords.back();
		m_MachineCoords.push_back(lastPoint + v);
		m_Length += ar::mat::Length(v);
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