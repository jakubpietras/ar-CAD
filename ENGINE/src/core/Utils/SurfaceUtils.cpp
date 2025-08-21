#include "arpch.h"
#include "SurfaceUtils.h"

namespace ar
{
	

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateRectangleC0Data(SurfaceDesc desc, mat::Vec3 origin)
	{
		auto points = std::vector<mat::Vec3>();

		// distance between neighboring points:
		float segmentWidth = desc.Dimensions.x / (desc.Size.u - 1),
			segmentHeight = desc.Dimensions.y / (desc.Size.v - 1);

		float posX = .0f, posZ = .0f;
		const float posY = origin.y;

		for (int z = 0; z < desc.Size.v; z++)
		{
			for (int x = 0; x < desc.Size.u; x++)
			{
				posX = origin.x + segmentWidth * x;
				posZ = origin.z + segmentHeight * z;
				points.emplace_back(posX, posY, posZ);
			}
		}
		return points;
	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateCylinderC0Data(SurfaceDesc desc, mat::Vec3 origin)
	{
		throw new std::runtime_error("Not implemented");

	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateRectangleC2Data(SurfaceDesc desc, mat::Vec3 origin)
	{
		throw new std::runtime_error("Not implemented");

	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateCylinderC2Data(SurfaceDesc desc, mat::Vec3 origin)
	{
		throw new std::runtime_error("Not implemented");

	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateSurfaceData(SurfaceDesc desc, mat::Vec3 origin)
	{
		switch (desc.Type)
		{
		case SurfaceType::RECTANGLEC0:
			return GenerateRectangleC0Data(desc, origin);
		case SurfaceType::CYLINDERC0:
			return GenerateCylinderC0Data(desc, origin);
		case SurfaceType::RECTANGLEC2:
			return GenerateRectangleC2Data(desc, origin);
		case SurfaceType::CYLINDERC2:
			return GenerateCylinderC2Data(desc, origin);
		default:
			return {};
		}
	}

	std::vector<uint32_t> SurfaceUtils::GenerateSurfaceIndices(SurfaceDesc desc)
	{
		switch (desc.Type)
		{
		case SurfaceType::RECTANGLEC0:
			return GenerateRectangleC0Indices(desc);
		case SurfaceType::CYLINDERC0:
			return GenerateRectangleC0Indices(desc);
		case SurfaceType::RECTANGLEC2:
			return GenerateSurfaceC2Indices(desc);
		case SurfaceType::CYLINDERC2:
			return GenerateSurfaceC2Indices(desc);
		default:
			return {};
		}

	}

	std::vector<uint32_t> SurfaceUtils::GenerateRectangleC0Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();
		
		for (int sv = 0; sv < desc.Segments.v; sv++)
		{
			for (int su = 0; su < desc.Segments.u; su++)
			{
				int base = (sv * 3) * desc.Size.u + (su * 3);
				for (int j = 0; j < 4; j++)
				{
					for (int i = 0; i < 4; i++)
						indices.push_back(base + j * desc.Size.u + i);
				}
			}
		}
		return indices;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateCylinderC0Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();

		int su = 0, sv = 0, base = 0;
		for (sv = 0; sv < desc.Segments.v; sv++)
		{
			for (su = 0; su < desc.Segments.u - 1; su++)
			{
				base = (sv * 3) * desc.Size.u + (su * 3);
				for (int j = 0; j < 4; j++)
				{
					for (int i = 0; i < 4; i++)
						indices.push_back(base + j * desc.Size.u + i);
				}
			}


		}
		return indices;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateSurfaceC2Indices(SurfaceDesc desc)
	{
		throw new std::runtime_error("Not implemented");
	}

}