#include "arpch.h"
#include "CurveUtils.h"

namespace ar
{
	bool CurveUtils::ValidateChain(Entity& entity)
	{
		auto& points = entity.GetComponent<ControlPointsComponent>().Points;
		return points.size() > 1;
	}

	bool CurveUtils::ValidateBezier(Entity& entity)
	{
		auto& points = entity.GetComponent<ControlPointsComponent>().Points;
		return points.size() > 1;
	}

	bool CurveUtils::ValidateCurve(Entity& entity)
	{
		auto& points = entity.GetComponent<ControlPointsComponent>().Points;
		return points.size() > 1;
	}

	std::vector<uint32_t> CurveUtils::GenerateC0Indices(size_t pointCount)
	{
		size_t index = 0;
		std::vector<uint32_t> indices;
		while (index < pointCount)
		{
			auto remaining = pointCount - index;
			uint32_t i = static_cast<uint32_t>(index);
			if (remaining > 3)
			{
				indices.insert(indices.end(), { i, i + 1, i + 2, i + 3 });
				index += 3;
			}
			else if (remaining > 2)
			{
				indices.insert(indices.end(), { i, i + 1, i + 2, i + 2 });
				break;
			}
			else if (remaining > 1)
			{
				indices.insert(indices.end(), { i, i, i + 1, i + 1 });
				break;
			}
			else
				break;
		}
		return indices;
	}

}