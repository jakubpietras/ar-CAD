#include "arpch.h"
#include "SurfaceUtils.h"

namespace ar
{
	

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateRectangleC0Data(SurfaceDesc desc, mat::Vec3 origin)
	{
		auto points = std::vector<mat::Vec3>();
		int pointsU = 3 * desc.Samples.u + 1,
			pointsV = 3 * desc.Samples.v + 1;
		float segmentWidth = static_cast<float>(desc.Size.u) / (pointsU - 1),
			segmentHeight = static_cast<float>(desc.Size.v) / (pointsV - 1);

		float posX = .0f, posZ = .0f;
		const float posY = origin.y;

		for (int z = 0; z < pointsV; z++)
		{
			for (int x = 0; x < pointsU; x++)
			{
				posX = origin.x + segmentWidth * x;
				posZ = origin.z + segmentHeight * z;
				points.emplace_back(posX, posY, posZ);
			}
		}
		return points;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateSurfaceC0Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();
		int pointsU = 3 * desc.Samples.u + 1,
			pointsV = 3 * desc.Samples.v + 1;
		
		for (int sv = 0; sv < desc.Samples.v; sv++)
		{
			for (int su = 0; su < desc.Samples.u; su++)
			{
				int base = (sv * 3) * pointsU + (su * 3);
				for (int j = 0; j < 4; j++)
				{
					for (int i = 0; i < 4; i++)
						indices.push_back(base + j * pointsU + i);
				}
			}
			//indices.push_back(row * pointsU);				// first point repeated for wrapping
		}
		return indices;
	}

}