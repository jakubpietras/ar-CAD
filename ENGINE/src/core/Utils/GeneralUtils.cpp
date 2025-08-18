#pragma once
#include "arpch.h"
#include "GeneralUtils.h"

namespace ar
{

	std::vector<ar::VertexPositionID> GeneralUtils::GetVertexData(std::vector<Entity> points, uint32_t id)
	{
		std::vector<VertexPositionID> data;
		data.reserve(points.size());
		std::transform(
			points.begin(), points.end(),
			std::back_inserter(data),
			[id](Entity& obj) {return ar::VertexPositionID(obj.GetComponent<TransformComponent>().Translation, id); }
		);
		return data;
	}

}