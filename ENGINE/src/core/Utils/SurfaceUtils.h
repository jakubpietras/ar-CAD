#pragma once
#include "core/Core.h"
#include "core/Renderer/VertexTypes.h"

namespace ar
{

	enum class SurfaceType
	{
		RECTANGLEC0,
		CYLINDERC0,
		RECTANGLEC2,
		CYLINDERC2
	};

	struct SurfaceDesc
	{
		ar::mat::Vec2	Dimensions{ 1.f, 1.f };	// Measure along the surface in u and v (extent)
												// (for cylinders, x is radius, y is height)
		UInt2			Size{ 16, 16 },			// Number of CONTROL POINTS in u and v (use internally)
						Samples{ 4, 4 },		// Density of the mesh
						Segments{ 1, 1 };		// Number of SEGMENTS in u and v
		SurfaceType		Type;
	};

	class SurfaceUtils
	{
	public:
		static std::vector<mat::Vec3> GenerateSurfaceData(SurfaceDesc desc, mat::Vec3 origin);
		static std::vector<uint32_t> GenerateSurfaceIndices(SurfaceDesc desc);
		static std::vector<uint32_t> GenerateControlMeshIndices(uint32_t pointsU, uint32_t pointsV);
		
	private:
		static std::vector<mat::Vec3> GenerateRectangleC0Data(SurfaceDesc desc, mat::Vec3 origin);
		static std::vector<mat::Vec3> GenerateCylinderC0Data(SurfaceDesc desc, mat::Vec3 origin);
		static std::vector<mat::Vec3> GenerateRectangleC2Data(SurfaceDesc desc, mat::Vec3 origin);
		static std::vector<mat::Vec3> GenerateCylinderC2Data(SurfaceDesc desc, mat::Vec3 origin);

		static std::vector<uint32_t> GenerateRectangleC0Indices(SurfaceDesc desc);
		static std::vector<uint32_t> GenerateCylinderC0Indices(SurfaceDesc desc);
		static std::vector<uint32_t> GenerateSurfaceC2Indices(SurfaceDesc desc);
		static std::vector<uint32_t> GenerateCylinderC2Indices(SurfaceDesc desc);
	};
}