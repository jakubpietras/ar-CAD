#pragma once
#include "core/Core.h"
#include "core/Renderer/Texture.h"
#include "ARMAT.h"
#include "core/Drawing/PaintSurface.h"

namespace ar
{
	class IntersectUtils
	{
	public:
		static ar::Ref<PaintSurface> CreateParameterImage(std::vector<ar::mat::Vec4> params,
			bool useFirst, size_t width, size_t height, bool isWrappedU, bool isWrappedV);

	private:
		static constexpr unsigned char FILLED_RED = 128;
		static constexpr unsigned char FILLED_GREEN = 128;
		static constexpr unsigned char FILLED_BLUE = 255;
		static constexpr unsigned char CURVE_RED = 0;
		static constexpr unsigned char CURVE_GREEN = 0;
		static constexpr unsigned char CURVE_BLUE = 0;


		static std::pair<int, int> FindSeed(mat::Vec2 p1, mat::Vec2 p2, ar::Ref<PaintSurface> tex, bool wrapU, bool wrapV);
	};
}

