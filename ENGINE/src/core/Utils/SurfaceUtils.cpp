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
		AR_ASSERT(desc.Segments.u >= 3, "Cylinder requires at least 3 segments!");
		auto points = std::vector<mat::Vec3>();
		float distAlong = desc.Dimensions.y / desc.Size.v;
		float theta = mat::Radians(360.0f / desc.Size.u);

		for (int j = 0; j < desc.Size.v; j++)
		{
			float posZ = origin.z + distAlong * j;
			for (int i = 0; i < desc.Size.u; i++)
			{
				float posX = origin.x + desc.Dimensions.x * cos(theta * i);
				float posY = origin.y + desc.Dimensions.x * sin(theta * i);
				points.emplace_back(posX, posY, posZ);
			}
		}
		return points;
	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateRectangleC2Data(SurfaceDesc desc, mat::Vec3 origin)
	{
		auto points = std::vector<mat::Vec3>();
		float segmentWidth = desc.Dimensions.x / desc.Segments.u,
			segmentHeight = desc.Dimensions.y / desc.Segments.v;

		float posX = 0.0f, posZ = 0.0f;
		const float posY = origin.y;

		for (int z = -1; z < desc.Size.v - 1; z++)
		{
			for (int x = -1; x < desc.Size.u - 1; x++)
			{
				posX = origin.x + segmentWidth * x;
				posZ = origin.z + segmentHeight * z;
				points.emplace_back(posX, posY, posZ);
			}
		}
		return points;
	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GenerateCylinderC2Data(SurfaceDesc desc, mat::Vec3 origin)
	{
		AR_ASSERT(desc.Segments.u >= 3, "Cylinder requires at least 3 segments!");

		auto points = std::vector<mat::Vec3>();
		float distAlong = desc.Dimensions.y / desc.Segments.v;
		float theta = mat::Radians(360.0f / desc.Size.u);
		float R = 3 * desc.Dimensions.x / (cos(theta) + 2);

		for (int j = 0; j < desc.Size.v; j++)
		{
			float posZ = origin.z + distAlong * j;
			for (int i = 0; i < desc.Size.v; i++)
			{
				float posX = origin.x + R * cos(theta * i);
				float posY = origin.y + R * sin(theta * i);
				points.emplace_back(posX, posY, posZ);
			}
		}
		return points;
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

	std::vector<uint32_t> SurfaceUtils::GenerateControlMeshIndices(uint32_t pointsU, uint32_t pointsV)
	{
		std::vector<uint32_t> indices;
		for (int v = 0; v < pointsV; v++)
		{
			for (int u = 0; u < pointsU; u++)
			{
				uint32_t index = v * pointsU + u;
				if (u == 0 || u == pointsU - 1)
					indices.push_back(index);
				else
					indices.insert(indices.end(), { index, index });
			}	
		}

		for (int u = 0; u < pointsU; u++)
		{
			for (int v = 0; v < pointsV; v++)
			{
				uint32_t index = v * pointsU + u;
				if (v == 0 || v == pointsV - 1)
					indices.push_back(index);
				else
					indices.insert(indices.end(), { index, index });
			}
		}
		
		return indices;
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

		for (int sv = 0; sv < desc.Segments.v; sv++)
		{
			for (int su = 0; su < desc.Segments.u; su++)
			{
				int base = (sv) * desc.Size.u + (su);
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

	std::vector<uint32_t> SurfaceUtils::GenerateCylinderC2Indices(SurfaceDesc desc)
	{
		throw new std::runtime_error("Not implemented");
	}

}