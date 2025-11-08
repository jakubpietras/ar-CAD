#pragma once
#include <vector>
#include "core/Scene/Entity.h"

namespace ar
{
	class HeightmapGenerator
	{
	public:
		struct HeightmapDesc
		{
			ar::mat::Vec2 LowerLeftCorner = {-7.5, 7.5};
			float RealWidth = 15.f, RealHeight = 15.f;
			uint32_t SamplesX = 100, SamplesY = 100;
			uint32_t SurfaceSamples = 100;
			float MinHeight = 0.f;
		};

		static std::vector<float> Generate(HeightmapDesc desc, std::vector<ar::Entity> objects);
	private:
		static ar::mat::Vec2T<int> MapPoint(HeightmapDesc desc, ar::mat::Vec3d point);
	};
}