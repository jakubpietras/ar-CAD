#include "arpch.h"
#include "SurfaceUtils.h"
#include "core/Scene/Entity.h"
#include "core/Scene/Components.h"
#include "core/Utils/GeneralUtils.h"

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

		for (size_t z = 0; z < desc.Size.v; z++)
		{
			for (size_t x = 0; x < desc.Size.u; x++)
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

		for (size_t j = 0; j < desc.Size.v; j++)
		{
			float posZ = origin.z + distAlong * j;
			for (size_t i = 0; i < desc.Size.u; i++)
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

		for (size_t j = 0; j < desc.Size.v; j++)
		{
			float posZ = origin.z + distAlong * j;
			for (size_t i = 0; i < desc.Size.u; i++)
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
		auto GetIndex = [&](uint32_t u, uint32_t v) -> uint32_t {
			return v * desc.Size.u + u;
		};
		std::vector<ar::Entity> refs;
		for (uint32_t v = 0; v < desc.Size.v; v++)
		{
			for (uint32_t u = 0; u < desc.Size.u; u++)
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
		auto getIndex = [&](uint32_t u, uint32_t v) -> uint32_t {
			return v * desc.Size.u + u;
			};

		if (desc.Type == SurfaceType::RECTANGLEC0 || desc.Type == SurfaceType::CYLINDERC0)
		{
			for (uint32_t segmentV = 0; segmentV < desc.Segments.v; segmentV++)
			{
				for (uint32_t segmentU = 0; segmentU < desc.Segments.u; segmentU++)
				{
					uint32_t baseU = segmentU * 3;
					uint32_t baseV = segmentV * 3;

					for (uint32_t localV = 0; localV < 4; localV++)
					{
						for (uint32_t localU = 0; localU < 4; localU++)
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
			for (uint32_t segmentV = 0; segmentV < desc.Segments.v; segmentV++)
			{
				for (uint32_t segmentU = 0; segmentU < desc.Segments.u; segmentU++)
				{
					uint32_t baseU = segmentU;
					uint32_t baseV = segmentV;

					for (uint32_t localV = 0; localV < 4; localV++)
					{
						for (uint32_t localU = 0; localU < 4; localU++)
						{
							indices.push_back(getIndex(baseU + localU, baseV + localV));
						}
					}
				}
			}
		}

		return indices;
	}

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
		}
		return desc;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateControlMeshIndices(SurfaceDesc desc)
	{
		std::vector<uint32_t> indices;
		uint32_t sizeU = desc.Size.u, sizeV = desc.Size.v;
		// Horizontal lines
		for (int v = 0; v < sizeV; v++)
		{
			for (int u = 0; u < sizeU - 1; u++)
			{
				indices.push_back(v * sizeU + u);
				indices.push_back(v * sizeU + u + 1);
			}
		}
		// Vertical lines
		for (int u = 0; u < sizeU; u++)
		{
			for (int v = 0; v < sizeV - 1; v++)
			{
				indices.push_back(v * sizeU + u);
				indices.push_back((v + 1) * sizeU + u);
			}
		}
		return indices;
	}

	std::vector<ar::Entity> SurfaceUtils::GetSegmentPoints(ar::Entity surface, mat::Vec2 segment)
	{
		auto& desc = surface.GetComponent<ar::SurfaceComponent>().Description;
		auto& ctrlPoints = surface.GetComponent<ar::ControlPointsComponent>().Points;
		std::vector<ar::Entity> points;
		points.reserve(16);
		size_t startU, startV;
		if (desc.Type == SurfaceType::RECTANGLEC0 || desc.Type == SurfaceType::CYLINDERC0)
		{
			startU = 3 * segment.x;
			startV = 3 * segment.y;
		} else
		{
			startU = segment.x;
			startV = segment.y;
		}

		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				size_t index = (startV + j) * desc.Size.u + (startU + i);
				points.push_back(ctrlPoints[index]);
			}
		}
		return points;
	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GetSegmentPointsBezier(std::vector<ar::mat::Vec3>& points, SurfaceDesc desc, mat::Vec2 segment)
	{
		std::vector<ar::mat::Vec3> segmentPoints;
		segmentPoints.reserve(16);
		size_t startU = 3 * segment.x, startV = 3 * segment.y;
		size_t bezierGridWidth = 3 * desc.Segments.u + 1;

		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				size_t index = (startV + j) * bezierGridWidth + (startU + i);
				segmentPoints.push_back(points[index]);
			}
		}
		return segmentPoints;
	}

	std::vector<ar::mat::Vec3> SurfaceUtils::GetBezierFromDeBoor(ar::Entity surface)
	{
		auto& surfComp = surface.GetComponent<ar::SurfaceComponent>();
		auto desc = surfComp.Description;
		auto segU = desc.Segments.u, segV = desc.Segments.v;
		auto points = ar::GeneralUtils::GetPos(surface.GetComponent<ar::ControlPointsComponent>().Points);
		std::vector<ar::mat::Vec3> bezier;

		std::vector<std::array<ar::mat::Vec3, 16>> convertedPatches;
		for (size_t v = 0; v < segV; v++)
		{
			for (size_t u = 0; u < segU; u++)
			{
				std::array<ar::mat::Vec3, 16> patch;
				for (size_t j = 0; j < 4; j++)
					for (size_t i = 0; i < 4; i++)
					{
						int index = (v + j) * desc.Size.u + (u + i);
						patch[j * 4 + i] = points[index];
					}
				std::array<ar::mat::Vec3, 16> p, B;
				for (int y = 0; y < 4; y++)
				{
					p[y * 4 + 0] = (patch[y * 4 + 0] + 4.0 * patch[y * 4 + 1] + patch[y * 4 + 2]) / 6.0f;
					p[y * 4 + 1] = (4.0 * patch[y * 4 + 1] + 2.0 * patch[y * 4 + 2]) / 6.0f;
					p[y * 4 + 2] = (2.0 * patch[y * 4 + 1] + 4.0 * patch[y * 4 + 2]) / 6.0f;
					p[y * 4 + 3] = (patch[y * 4 + 1] + 4.0 * patch[y * 4 + 2] + patch[y * 4 + 3]) / 6.0f;
				}
				for (int x = 0; x < 4; x++)
				{
					B[0 * 4 + x] = (p[0 * 4 + x] + 4.0 * p[1 * 4 + x] + p[2 * 4 + x]) / 6.0f;
					B[1 * 4 + x] = (4.0 * p[1 * 4 + x] + 2.0 * p[2 * 4 + x]) / 6.0f;
					B[2 * 4 + x] = (2.0 * p[1 * 4 + x] + 4.0 * p[2 * 4 + x]) / 6.0f;
					B[3 * 4 + x] = (p[1 * 4 + x] + 4.0 * p[2 * 4 + x] + p[3 * 4 + x]) / 6.0f;
				}

				convertedPatches.push_back(B);
			}
		}

		// todo: stitch a surface
		for (size_t v = 0; v < segV; v++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				// Skip first row of interior patches
				if (v > 0 && j == 0) continue;

				for (size_t u = 0; u < segU; u++)
				{
					auto& patch = convertedPatches[v * segU + u];

					for (size_t i = 0; i < 4; i++)
					{
						// Skip first column of interior patches
						if (u > 0 && i == 0) continue;

						bezier.push_back(patch[j * 4 + i]);
					}
				}
			}
		}
		return bezier;
	}


	bool SurfaceUtils::IsSurfaceC0Cylinder(std::vector<ar::Entity>& points, mat::UInt2 size)
	{
		// Check if first and last column are the same (same internal ID)
		bool wrappedU = true, wrappedV = true;
		for (size_t v = 0; v < size.v; v++)
		{
			size_t base = v * size.u;
			if (points[base].GetID() != points[base + (size.u - 1)].GetID())
			{
				wrappedU = false;
				break;
			}
		}
		for (size_t u = 0; u < size.u; u++)
		{
			size_t base = u * size.v;
			if (points[base].GetID() != points[base + (size.v - 1)].GetID())
			{
				wrappedV = false;
				break;
			}
		}
		return wrappedU || wrappedV;
	}

	bool SurfaceUtils::IsSurfaceC2Cylinder(std::vector<ar::Entity>& points, mat::UInt2 size)
	{
		// Check if three first and three last columns are the same (same internal ID)
		bool wrappedU = true, wrappedV = true;
		for (size_t v = 0; v < size.v; v++)
		{
			size_t base = v * size.u;
			if (points[base + 2].GetID() != points[base + (size.u - 1)].GetID()
				|| points[base + 1].GetID() != points[base + (size.u - 2)].GetID()
				|| points[base].GetID() != points[base + (size.u - 3)].GetID())
			{
				wrappedU = false;
				break;
			}
		}
		for (size_t u = 0; u < size.u; u++)
		{
			size_t base = u * size.v;
			if (points[base + 2].GetID() != points[base + (size.v - 1)].GetID()
				|| points[base + 1].GetID() != points[base + (size.v - 2)].GetID()
				|| points[base].GetID() != points[base + (size.v - 3)].GetID())
			{
				wrappedV = false;
				break;
			}
		}
		return wrappedU || wrappedV;
	}

	std::vector<uint32_t> SurfaceUtils::GenerateRectangleC0Indices(SurfaceDesc desc)
	{
		auto indices = std::vector<uint32_t>();
		auto getIndex = [&](uint32_t u, uint32_t v) -> uint32_t {
			return v * desc.Size.u + u;
		};

		for (uint32_t segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (uint32_t segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				uint32_t baseU = segmentU * 3;
				uint32_t baseV = segmentV * 3;

				for (uint32_t localV = 0; localV < 4; localV++)
				{
					for (uint32_t localU = 0; localU < 4; localU++)
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

		auto getIndex = [&](uint32_t u, uint32_t v) -> uint32_t {
			uint32_t wrappedU = u % (desc.Size.u);
			return v * desc.Size.u + wrappedU;
			};

		for (uint32_t segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (uint32_t segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				uint32_t baseU = segmentU * 3;
				uint32_t baseV = segmentV * 3;

				for (uint32_t localV = 0; localV < 4; localV++)
				{
					for (uint32_t localU = 0; localU < 4; localU++)
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
		auto getIndex = [&](uint32_t u, uint32_t v) -> uint32_t {
			return v * desc.Size.u + u;
			};

		for (uint32_t segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (uint32_t segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				uint32_t baseU = segmentU;
				uint32_t baseV = segmentV;

				for (uint32_t localV = 0; localV < 4; localV++)
				{
					for (uint32_t localU = 0; localU < 4; localU++)
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

		auto getIndex = [&](uint32_t u, uint32_t v) -> uint32_t {
			uint32_t wrappedU = u % (desc.Size.u);
			return v * desc.Size.u + wrappedU;
			};

		for (uint32_t segmentV = 0; segmentV < desc.Segments.v; segmentV++)
		{
			for (uint32_t segmentU = 0; segmentU < desc.Segments.u; segmentU++)
			{
				uint32_t baseU = segmentU;
				uint32_t baseV = segmentV;

				for (uint32_t localV = 0; localV < 4; localV++)
				{
					for (uint32_t localU = 0; localU < 4; localU++)
					{
						indices.push_back(getIndex(baseU + localU, baseV + localV));
					}
				}
			}
		}
		return indices;
	}

}