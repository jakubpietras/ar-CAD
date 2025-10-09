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

	std::vector<ar::VertexPosition> GeneralUtils::GetVertexData(std::vector<ar::mat::Vec3> positions)
	{
		std::vector<VertexPosition> data;
		data.reserve(positions.size());
		for (auto& position : positions)
			data.push_back({ position });
		return data;
	}

	std::vector<ar::mat::Vec3> GeneralUtils::GetPos(std::vector<ar::Entity>& objects)
	{
		std::vector<ar::mat::Vec3> positions;
		for (auto& object : objects)
			positions.push_back(object.GetComponent<ar::TransformComponent>().Translation);
		return positions;
	}

	std::vector<mat::Vec3d> GeneralUtils::GetPosD(std::vector<ar::Entity>& objects)
	{
		std::vector<mat::Vec3d> positions;
		for (auto& object : objects)
		{
			auto t = object.GetComponent<ar::TransformComponent>().Translation;
			positions.push_back({ t.x, t.y, t.z });
		}
		return positions;
	}

	bool GeneralUtils::IsWrappedU(ar::Entity e)
	{
		if (e.HasAnyComponent<ar::TorusComponent, ar::SurfaceComponent>())
			return true;
		return false;
	}

	bool GeneralUtils::IsWrappedV(ar::Entity e)
	{
		if (e.HasComponent<ar::TorusComponent>())
			return true;
		return false;
	}

	std::vector<ar::Entity> GeneralUtils::CreateScenePoints(std::vector<ar::mat::Vec3> positions, ar::SceneFactory& factory)
	{
		std::vector<ar::Entity> points;
		for (auto& pos : positions)
		{
			auto point = factory.CreatePoint(pos, std::nullopt, "Point");
			points.push_back(point);
		}
		return points;
	}

	std::vector<mat::Vec3> GeneralUtils::VecDoubleToFloat(std::vector<ar::mat::Vec3d> vec)
	{
		std::vector<mat::Vec3> out;
		out.reserve(vec.size());
		for (auto& v : vec)
			out.emplace_back(static_cast<mat::Vec3>(v)); // uses converting constructor
		return out;
	}

	std::vector<mat::Vec4> GeneralUtils::VecDoubleToFloat(std::vector<ar::mat::Vec4d> vec)
	{
		std::vector<mat::Vec4> out;
		out.reserve(vec.size());
		for (auto& v : vec)
			out.emplace_back(static_cast<mat::Vec4>(v)); // uses converting constructor
		return out;
	}

}