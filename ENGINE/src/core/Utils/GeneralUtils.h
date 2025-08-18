#pragma once
#include <vector>
#include "core/Renderer/VertexTypes.h"
#include "core/Scene/Entity.h"

namespace ar
{
	class GeneralUtils
	{
	public:
		static std::vector<VertexPositionID> GetVertexData(std::vector<Entity> points, uint32_t id);

	};
}