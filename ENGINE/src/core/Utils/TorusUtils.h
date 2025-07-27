#pragma once
#include <vector>
#include "core/Renderer/VertexTypes.h"


namespace ar
{
	struct TorusDesc
	{
		float		SmallRadius = 1.0f,
					LargeRadius = 2.0f;
		uint32_t	SamplesU = 16,
					SamplesV = 16;
	};

	class TorusUtils
	{
	public:
		static std::vector<VertexPosition> GenerateTorusVertices(const TorusDesc& desc);
		static std::vector<std::vector<uint32_t>> GenerateTorusEdges(const TorusDesc& desc);

	};
}