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
			uint32_t SamplesX = 1000, SamplesY = 1000;
			uint32_t SurfaceSamples = 2000;
			float MinHeight = 0.f;
		};

		static std::vector<float> Generate(HeightmapDesc desc, std::vector<ar::Entity> objects);
		static ar::mat::Vec2T<int> MapPoint(HeightmapDesc desc, ar::mat::Vec3d point);
		static ar::mat::Vec2T<int> MapPoint(HeightmapDesc desc, ar::mat::Vec3 point);
	};
}