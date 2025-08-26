#pragma once
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include "core/Scene/SceneFactory.h"

using json = nlohmann::ordered_json;

namespace ar
{
	class SceneImporter
	{
	public:
		static bool Import(const std::string& filepath, SceneFactory& factory);
	private:
		static std::unordered_map<uint32_t, Entity> m_PointMap;

		static void LoadPoints(json pointData, SceneFactory& factory);
		static void LoadGeometry(json geometryData, SceneFactory& factory);

		static Entity ImportPoint(json point, SceneFactory& factory);
		static Entity ImportTorus(json torus, SceneFactory& factory);
		static Entity ImportChain(json chain, SceneFactory& factory);
		static Entity ImportBezierC0(json bezierC0, SceneFactory& factory);
		static Entity ImportBezierC2(json bezierC2, SceneFactory& factory);
		static Entity ImportInterpolatedC2(json intC2, SceneFactory& factory);
		static Entity ImportSurfaceC0(json surface, SceneFactory& factory);
		static Entity ImportSurfaceC2(json surface, SceneFactory& factory);

		static mat::Vec3 LoadVec3(json data);
		static mat::UInt2 LoadUInt2(json data);
		static mat::Quat LoadQuat(json data);

		static bool LoadJsonFromFile(const std::string& filepath, json& output);
		static std::vector<Entity> LoadControlPoints(json data);

	};
}

