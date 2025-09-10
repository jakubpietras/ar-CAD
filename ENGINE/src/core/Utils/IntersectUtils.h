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
		static ar::Ref<PaintSurface> CreateParameterImage(std::vector<ar::mat::Vec4> params, bool useFirst, size_t width, size_t height);
	};
}

