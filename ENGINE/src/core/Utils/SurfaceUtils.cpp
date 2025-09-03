#include "arpch.h"
#include "SurfaceUtils.h"
#include "core/Scene/Entity.h"
#include "core/Scene/Components.h"

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

		for (int z = -1; z < (int)desc.Size.v - 1; z++)
		{
			for (int x = -1; x < (int)desc.Size.u - 1; x++)
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
			for (int i = 0; i < desc.Size.u; i++)
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
			return GenerateCylinderC0Indices(desc);
		case SurfaceType::RECTANGLEC2:
			return GenerateRectangleC2Indices(desc);
		case SurfaceType::CYLINDERC2:
			return GenerateCylinderC2Indices(desc);
		default:
			return {};
		}

	}

	std::vector<ar::Entity> SurfaceUtils::GeneratePointReferences(SurfaceDesc desc, std::vector<ar::Entity> points)
	{
		if (desc.Type == SurfaceType::RECTANGLEC0 || desc.Type == SurfaceType::RECTANGLEC2)
			return points;
		
		// for cylinders, the first column is repeated as last
		auto GetIndex = [&](int u, int v) -> uint32_t {
			return v * desc.Size.u + u;
		};
		std::vector<ar::Entity> refs;
		for (int v = 0; v < desc.Size.v; v++)
		{
			for (int u = 0; u < desc.Size.u; u++)
				refs.push_back(points[GetIndex(u, v)]);
			if (desc.Type == SurfaceType::CYLINDERC0)
				refs.push_back(points[GetIndex(0, v)]);
			else if (desc.Type == SurfaceType::CYLINDERC2)
			{
				refs.push_back(points[GetIndex(0, v)]);
				refs.push_back(points[GetIndex(1, v)]);
				refs.push_back(points[GetIndex(2, v)]);
			}
		}
		return refs;
	}
	std::vector<uint32_t> SurfaceUtils::GenerateSurfaceRefIndices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();
		auto getIndex = [&](int u, int v) -> uint32_t {
			return v * desc.Size.u + u;
			};

		if (desc.Type == SurfaceType::RECTANGLEC0 || desc.Type == SurfaceType::CYLINDERC0)
		{
			for (int segmentV = 0; segmentV < desc.Segments.v; segmentV++)
			{
				for (int segmentU = 0; segmentU < desc.Segments.u; segmentU++)
				{
					int baseU = segmentU * 3;
					int baseV = segmentV * 3;

					for (int localV = 0; localV < 4; localV++)
					{
						for (int localU = 0; localU < 4; localU++)
						{
							indices.push_back(getIndex(baseU + localU, baseV + localV));
						}
					}
				}
			}
		}
		else
		{
			// todo: fix for c2
			for (int segmentV = 0; segmentV < desc.Segments.v; segmentV++)
			{
				for (int segmentU = 0; segmentU < desc.Segments.u; segmentU++)
				{
					int baseU = segmentU;
					int baseV = segmentV;

					for (int localV = 0; localV < 4; localV++)
					{
						for (int localU = 0; localU < 4; localU++)
						{
							indices.push_back(getIndex(baseU + localU, baseV + localV));
						}
					}
				}
			}
		}

		return indices;
	}
	//std::vector<uint32_t> SurfaceUtils::GenerateSurfaceRefIndices(SurfaceDesc desc)
	//{
	//	auto indices = std::vector<uint32_t>();
	//	auto getIndex = [&](int u, int v) -> uint32_t {
	//		return v * desc.Size.u + u;
	//		};

	//	if (desc.Type == SurfaceType::RECTANGLEC0 || desc.Type == SurfaceType::CYLINDERC0)
	//	{
	//		for (int segmentV = 0; segmentV * 3 < desc.Size.v; segmentV++)
	//		{
	//			for (int segmentU = 0; segmentU * 3 < desc.Size.u; segmentU++)
	//			{
	//				int baseU = segmentU * 3;
	//				int baseV = segmentV * 3;

	//				for (int localV = 0; localV < 4; localV++)
	//				{
	//					for (int localU = 0; localU < 4; localU++)
	//					{
	//						indices.push_back(getIndex(baseU + localU, baseV + localV));
	//					}
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		// todo: fix for c2
	//		for (int segmentV = 0; segmentV * 3 < desc.Size.v; segmentV++)
	//		{
	//			for (int segmentU = 0; segmentU * 3 < desc.Size.u; segmentU++)
	//			{
	//				int baseU = segmentU * 3;
	//				int baseV = segmentV * 3;

	//				for (int localV = 0; localV < 4; localV++)
	//				{
	//					for (int localU = 0; localU < 4; localU++)
	//					{
	//						indices.push_back(getIndex(baseU + localU, baseV + localV));
	//					}
	//				}
	//			}
	//		}
	//	}

	//	return indices;
	//}

	ar::SurfaceDesc SurfaceUtils::AdjustSurfaceDescription(SurfaceDesc desc)
	{
		switch (desc.Type)
		{
		case SurfaceType::CYLINDERC0:
			desc.Size = { desc.Size.u + 1, desc.Size.v };
			break;
		case SurfaceType::CYLINDERC2:
			desc.Size = { desc.Size.u + 3, desc.Size.v };
			break;
		default:
			return desc;
		}
	}

	std::vector<uint32_t> SurfaceUtils::GenerateControlMeshIndices(SurfaceDesc desc, std::vector<uint32_t> meshIndices)
	{
		std::vector<uint32_t> indices;
		int step = (desc.Type == SurfaceType::CYLINDERC0 || desc.Type == SurfaceType::RECTANGLEC0) ? 3 : 1;
		
		auto getIndex = [&](uint32_t u, uint32_t v) -> uint32_t {
			return v * desc.Size.u + u;
		};
		for (uint32_t sv = 0; sv < desc.Segments.v; sv++)
		{
			for (uint32_t su = 0; su < desc.Segments.u; su++)
			{
				int baseU = su * step;
				int baseV = sv * step;
				for (uint32_t j = 0; j < 4; j++)
				{
					for (uint32_t i = 0; i < 4; i++)
					{
						auto index = meshIndices[getIndex(baseU + i, baseV + j)];
						if (i == 0 || i == 3)
							indices.push_back(index);
						else
							indices.insert(indices.begin(), { index, index });
					}
				}
			}
		}
		return indices;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateRectangleC0Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();
		auto getIndex = [&](int u, int v) -> uint32_t {
			return v * desc.Size.u + u;
		};

		for (int segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (int segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				int baseU = segmentU * 3;
				int baseV = segmentV * 3;

				for (int localV = 0; localV < 4; localV++)
				{
					for (int localU = 0; localU < 4; localU++)
					{
						indices.push_back(getIndex(baseU + localU, baseV + localV));
					}
				}
			}
		}
		return indices;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateCylinderC0Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();

		auto getIndex = [&](int u, int v) -> uint32_t {
			int wrappedU = u % (desc.Size.u);
			return v * desc.Size.u + wrappedU;
			};

		for (int segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (int segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				int baseU = segmentU * 3;
				int baseV = segmentV * 3;

				for (int localV = 0; localV < 4; localV++)
				{
					for (int localU = 0; localU < 4; localU++)
					{
						indices.push_back(getIndex(baseU + localU, baseV + localV));
					}
				}
			}
		}
		return indices;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateRectangleC2Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();
		auto getIndex = [&](int u, int v) -> uint32_t {
			return v * desc.Size.u + u;
			};

		for (int segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (int segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				int baseU = segmentU;
				int baseV = segmentV;

				for (int localV = 0; localV < 4; localV++)
				{
					for (int localU = 0; localU < 4; localU++)
					{
						indices.push_back(getIndex(baseU + localU, baseV + localV));
					}
				}
			}
		}
		return indices;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateCylinderC2Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();

		auto getIndex = [&](int u, int v) -> uint32_t {
			int wrappedU = u % (desc.Size.u);
			return v * desc.Size.u + wrappedU;
			};

		for (int segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (int segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				int baseU = segmentU;
				int baseV = segmentV;

				for (int localV = 0; localV < 4; localV++)
				{
					for (int localU = 0; localU < 4; localU++)
					{
						indices.push_back(getIndex(baseU + localU, baseV + localV));
					}
				}
			}
		}
		return indices;
	}

}