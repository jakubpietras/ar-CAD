#pragma once
#include "core/Scene/Entity.h"
#include "core/Paths/ToolType.h"
#include <filesystem>

namespace ar
{
	class ToolPath
	{
	public:
		ToolPath(ar::mat::Vec3 startPoint, ToolType type, float baseHeight = 1.5f);
		void MoveTo(ar::mat::Vec3 point);
		void MoveBy(ar::mat::Vec3 v);
		inline const ar::mat::Vec3& GetCurrentPos() { return m_MachineCoords.back(); }
		void ConvertToGCode(uint32_t order, std::filesystem::path path);
	private:
		std::vector<ar::mat::Vec3> m_MachineCoords{};
		float m_Length;
		ToolType m_ToolType;
		ar::mat::Vec3 m_BaseDisplacement;
	};
}