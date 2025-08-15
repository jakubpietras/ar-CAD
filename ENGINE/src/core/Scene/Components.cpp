#pragma once
#include "arpch.h"
#include "Components.h"
#include "Entity.h"

namespace ar
{

	ControlPointsComponent::ControlPointsComponent()
		: Points({}) { }


	ControlPointsComponent::ControlPointsComponent(std::vector<Entity> initialPoints)
		: Points(initialPoints) { }

	std::vector<ar::VertexPositionID> ControlPointsComponent::GetVertexData(uint32_t id)
	{
		std::vector<VertexPositionID> data;
		data.reserve(Points.size());
		std::transform(
			Points.begin(), Points.end(),
			std::back_inserter(data),
			[id](Entity& obj) {return ar::VertexPositionID(obj.GetComponent<TransformComponent>().Translation, id); }
		);
		return data;
	}

}