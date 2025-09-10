#include "arpch.h"
#include "IntersectUtils.h"
#include "core/Drawing/PaintSurface.h"

namespace ar
{

	ar::Ref<PaintSurface> IntersectUtils::CreateParameterImage(std::vector<ar::mat::Vec4> params, bool useFirst, size_t width, size_t height)
	{
		// todo: create parameter image of a curve
		PaintSurface image(width, height);

		float p1x, p1y, p2x, p2y;
		for (size_t i = 0; i < params.size() - 1; i++)
		{
			if (useFirst)
			{
				p1x = params[i].x * width;
				p1y = params[i].y * height;
				p2x = params[i + 1].x * width;
				p2y = params[i + 1].y * height;
			}
			else
			{
				p1x = params[i].z * width;
				p1y = params[i].w * height;
				p2x = params[i + 1].z * width;
				p2y = params[i + 1].w * height;
			}
			image.DrawLine(p1x, p1y, p2x, p2y, 0, 0, 0);
		}
		return std::make_shared<PaintSurface>(image);
	}

}