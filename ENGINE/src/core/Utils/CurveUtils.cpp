#include "arpch.h"
#include "CurveUtils.h"

namespace ar
{
	bool CurveUtils::ValidateChain(Entity& entity)
	{
		auto& points = entity.GetComponent<ControlPointsComponent>().Points;
		return points.size() > 1;
	}

}