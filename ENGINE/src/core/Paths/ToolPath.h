#pragma once
#include "core/Scene/Entity.h"

namespace ar
{
	class ToolPath
	{
	public:


	private:
		std::vector<ar::Entity> m_Surfaces{};
		std::vector<ar::mat::Vec3> m_MachineCoords{};
		uint32_t m_CommandCount = 0;


	};
}