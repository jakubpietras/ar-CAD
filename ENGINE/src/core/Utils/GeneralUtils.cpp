#pragma once
#include "arpch.h"
#include "GeneralUtils.h"
#include "core/Scene/Components.h"

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

	std::vector<ar::VertexPositionID> GeneralUtils::GetVertexData(std::vector<ar::mat::Vec3> positions, uint32_t id)
	{
		std::vector<VertexPositionID> data;
		data.reserve(positions.size());
		for (auto& position : positions)
			data.push_back({ position, id });
		return data;
	}

	std::vector<ar::mat::Vec3> GeneralUtils::GetPos(std::vector<ar::Entity>& objects)
	{
		std::vector<ar::mat::Vec3> positions;
		for (auto& object : objects)
			positions.push_back(object.GetComponent<ar::TransformComponent>().Translation);
		return positions;
	}

}