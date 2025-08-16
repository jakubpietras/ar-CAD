#pragma once
#include "core/Scene/Entity.h"

namespace ar
{
	class CurveUtils
	{
	public:
		static bool ValidateChain(Entity& entity);
		static bool ValidateBezier(Entity& entity);

		static bool ValidateCurve(Entity& entity);
		static std::vector<uint32_t> GenerateC0Indices(size_t pointCount);
	};
}