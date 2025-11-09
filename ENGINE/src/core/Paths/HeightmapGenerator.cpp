#include "arpch.h"
#include "HeightmapGenerator.h"
#include "core/Utils/Parametric.h"

namespace ar
{
	std::vector<float> HeightmapGenerator::Generate(HeightmapDesc desc, std::vector<ar::Entity> objects)
	{
		float step = 1.0f / desc.SurfaceSamples;
		std::vector<float> hm(desc.SamplesX * desc.SamplesY, desc.MinHeight);

		for (auto& object : objects)
		{
			auto surface = Parametric::Create(object);
			int numSamples = static_cast<int>(1.0f / step) + 1;
			for (int ii = 0; ii < numSamples; ii++) 
			{
				for (int jj = 0; jj < numSamples; jj++) 
				{
					float i = ii * step;
					float j = jj * step;
					auto point = surface->Evaluate(i, j);
					auto hmCoords = MapPoint(desc, point);
					if (hmCoords.x != -1 && hmCoords.y != -1)
					{
						auto index = hmCoords.y * desc.SamplesX + hmCoords.x;
						if (hm[index] < point.z) 
							hm[index] = point.z;
					}
				}
			}
		}
		return hm;
	}

	ar::mat::Vec2T<int> HeightmapGenerator::MapPoint(HeightmapDesc desc, ar::mat::Vec3d point)
	{
		ar::mat::Vec2T<int> mapped{ -1, -1 };
		auto cellHeight = desc.RealHeight / desc.SamplesY;
		auto cellWidth = desc.RealWidth / desc.SamplesX;

		auto pointX = point.x - desc.LowerLeftCorner.x;
		auto pointY = desc.LowerLeftCorner.y - point.y;
		if (pointX < 0 || pointX > desc.RealWidth)
			return mapped;
		if (pointY < 0 || pointY > desc.RealHeight)
			return mapped;

		mapped.x = std::min(static_cast<uint32_t>(std::floor(pointX / cellWidth)), desc.SamplesX - 1);
		mapped.y = std::min(static_cast<uint32_t>(std::floor(pointY / cellHeight)), desc.SamplesY - 1);
		return mapped;
	}

	ar::mat::Vec2T<int> HeightmapGenerator::MapPoint(HeightmapDesc desc, ar::mat::Vec3 point)
	{
		ar::mat::Vec2T<int> mapped{ -1, -1 };
		auto cellHeight = desc.RealHeight / desc.SamplesY;
		auto cellWidth = desc.RealWidth / desc.SamplesX;

		auto pointX = point.x - desc.LowerLeftCorner.x;
		auto pointY = desc.LowerLeftCorner.y - point.y;
		if (pointX < 0 || pointX > desc.RealWidth)
			return mapped;
		if (pointY < 0 || pointY > desc.RealHeight)
			return mapped;

		mapped.x = std::min(static_cast<uint32_t>(std::floor(pointX / cellWidth)), desc.SamplesX - 1);
		mapped.y = std::min(static_cast<uint32_t>(std::floor(pointY / cellHeight)), desc.SamplesY - 1);
		return mapped;
	}

}