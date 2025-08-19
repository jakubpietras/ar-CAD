#pragma once
#include "core/Core.h"
#include "core/Renderer/VertexTypes.h"

namespace ar
{
	struct SurfaceDesc
	{
		UInt2	Size{ 1, 1 },
				Samples{ 4, 4 };
		bool	IsCylinder = false;
	};

	class SurfaceUtils
	{
	public:
		static std::vector<mat::Vec3> GenerateRectangleC0Data(SurfaceDesc desc, mat::Vec3 origin);
		static std::vector<mat::Vec3> GenerateCylinderC0Data(SurfaceDesc desc, mat::Vec3 origin);
		static std::vector<mat::Vec3> GenerateRectangleC2Data(SurfaceDesc desc, mat::Vec3 origin);
		static std::vector<mat::Vec3> GenerateCylinderC2Data(SurfaceDesc desc, mat::Vec3 origin);

		static std::vector<uint32_t> GenerateSurfaceC0Indices(SurfaceDesc desc);
		static std::vector<uint32_t> GenerateSurfaceC2Indices(SurfaceDesc desc);

	};
}