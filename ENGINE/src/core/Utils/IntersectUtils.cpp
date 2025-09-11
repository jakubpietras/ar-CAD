#include "arpch.h"
#include "IntersectUtils.h"
#include "core/Drawing/PaintSurface.h"
#include <algorithm>

namespace ar
{

	ar::Ref<PaintSurface> IntersectUtils::CreateParameterImage(std::vector<ar::mat::Vec4> params,
		bool useFirst, size_t width, size_t height, bool isWrappedU, bool isWrappedV)
	{
		auto image = std::make_shared<PaintSurface>(width, height);

		// Draw lines
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
			image->DrawLine(p1x, p1y, p2x, p2y, CURVE_RED, CURVE_GREEN, CURVE_BLUE, isWrappedU, isWrappedV);
		}

		// Flood fill outside the lines
		mat::Vec2 p1, p2;

		if (useFirst)
		{
			p1 = { float(std::clamp(int(params[0].x * width), 0, int(width - 1))), float(std::clamp(int(params[0].y * height), 0, int(height - 1))) };
			p2 = { float(std::clamp(int(params[1].x * width), 0, int(width - 1))), float(std::clamp(int(params[1].y * height), 0, int(height - 1))) };
		}
		else
		{
			p1 = { float(std::clamp(int(params[0].z * width), 0, int(width - 1))), float(std::clamp(int(params[0].w * height), 0, int(height - 1))) };
			p2 = { float(std::clamp(int(params[1].z * width), 0, int(width - 1))), float(std::clamp(int(params[1].w * height), 0, int(height - 1))) };
		}
		auto seed = FindSeed(p1, p2, image, isWrappedU, isWrappedV);
		image->FloodFill(seed.first, seed.second, FILLED_RED, FILLED_GREEN, FILLED_BLUE, isWrappedU, isWrappedV);

		return image;
	}

	std::pair<int, int> IntersectUtils::FindSeed(mat::Vec2 p1, mat::Vec2 p2, ar::Ref<PaintSurface> tex, bool wrapU, bool wrapV)
	{
		auto dx = p1.x - p2.x;
		auto dy = p1.y - p2.y;
		auto length = sqrt(dx * dx + dy * dy);

		// Handle zero-length case
		if (length < 1e-6) {
			return { 0, 0 };
		}

		auto normX = -dy / length;
		auto normY = dx / length;

		for (int distance = 1; distance <= 10; distance++) {
			for (int side = -1; side <= 1; side += 2) { // -1 and +1
				// Calculate raw seed position (may be outside bounds)
				int sx = int(p1.x + normX * distance * side);
				int sy = int(p1.y + normY * distance * side);

				// Handle wrapping BEFORE clamping
				if (wrapU) {
					sx = sx % int(tex->GetWidth());
					if (sx < 0) sx += tex->GetWidth();
				}
				else {
					sx = std::clamp(sx, 0, int(tex->GetWidth() - 1));
				}

				if (wrapV) {
					sy = sy % int(tex->GetHeight());
					if (sy < 0) sy += tex->GetHeight();
				}
				else {
					sy = std::clamp(sy, 0, int(tex->GetHeight() - 1));
				}

				// Check if this pixel is not a curve
				if (!tex->IsSameColor(sx, sy, CURVE_RED, CURVE_GREEN, CURVE_BLUE))
				{
					return { sx, sy };
				}
			}
		}

		int sx = int(p1.x + normX * 5);
		int sy = int(p1.y + normY * 5);

		if (wrapU) {
			sx = sx % int(tex->GetWidth());
			if (sx < 0) sx += tex->GetWidth();
		}
		else {
			sx = std::clamp(sx, 0, int(tex->GetWidth() - 1));
		}

		if (wrapV) {
			sy = sy % int(tex->GetHeight());
			if (sy < 0) sy += tex->GetHeight();
		}
		else {
			sy = std::clamp(sy, 0, int(tex->GetHeight() - 1));
		}

		return { sx, sy };
	}

}