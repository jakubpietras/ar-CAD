#pragma once
#include <vector>
#include "core/Renderer/VertexTypes.h"


namespace ar
{
	struct TorusDesc
	{
		float		SmallRadius = 1.0f,
					LargeRadius = 2.0f;
		UInt2		Samples {17, 16};
	};

	class TorusUtils
	{
	public:
		static std::vector<VertexPositionID> GenerateTorusVertices(const TorusDesc& desc, uint32_t id);
		static std::vector<std::vector<uint32_t>> GenerateTorusEdges(const TorusDesc& desc);
	};
}