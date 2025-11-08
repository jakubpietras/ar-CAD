#pragma once
#include "core/Scene/Entity.h"
#include "core/Paths/ToolType.h"
#include <filesystem>

namespace ar
{
	class ToolPath
	{
	public:
		ToolPath(ar::mat::Vec3 startPoint, ToolType type);
		void AddCoordinate(ar::mat::Vec3 point);
		void ConvertToGCode(uint32_t order, std::filesystem::path path);
	private:
		std::vector<ar::mat::Vec3> m_MachineCoords{};
		float m_Length;
		ToolType m_ToolType;
	};
}