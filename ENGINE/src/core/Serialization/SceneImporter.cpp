#include "arpch.h"
#include "SceneImporter.h"
#include "core/Scene/Components.h"

namespace ar
{

	bool SceneImporter::Import(const std::string& filepath, SceneFactory& factory)
	{
		m_PointMap.clear();
		json data;
		auto ret = LoadJsonFromFile(filepath, data);
		if (!ret)
			return false;

		LoadPoints(data["points"], factory);
		LoadGeometry(data["geometry"], factory);

		return true;
	}

	std::unordered_map<uint32_t, Entity> SceneImporter::m_PointMap;

	void SceneImporter::LoadPoints(json pointData, SceneFactory& factory)
	{
		for (auto& point : pointData)
		{
			auto p = ImportPoint(point, factory);
			uint32_t id = point["id"].get<uint32_t>();
			m_PointMap[id] = p;
		}
	}

	void SceneImporter::LoadGeometry(json geometryData, SceneFactory& factory)
	{
		for (auto& geometry : geometryData)
		{
			auto type = geometry["objectType"].get<std::string>();
			if (type == "torus")
				ImportTorus(geometry, factory);
			else if (type == "chain")
				ImportChain(geometry, factory);
			else if (type == "bezierC0")
				ImportBezierC0(geometry, factory);
			else if (type == "bezierC2")
				ImportBezierC2(geometry, factory);
			else if (type == "interpolatedC2")
				ImportInterpolatedC2(geometry, factory);
			else if (type == "bezierSurfaceC0")
				ImportSurfaceC0(geometry, factory);
			else if (type == "bezierSurfaceC2")
				ImportSurfaceC2(geometry, factory);
		}
	}

	ar::Entity SceneImporter::ImportPoint(json point, SceneFactory& factory)
	{
		uint32_t id = point["id"];
		std::string name = (point.contains("name")) ? point["name"] : "Point";
		mat::Vec3 position = LoadVec3(point["position"]);
		return factory.CreatePoint(position, id, name);
	}

	ar::Entity SceneImporter::ImportTorus(json torus, SceneFactory& factory)
	{
		uint32_t id = torus["id"];
		std::string name = (torus.contains("name")) ? torus["name"] : "Torus";
		mat::Vec3 position = LoadVec3(torus["position"]);
		mat::Quat rotation = LoadQuat(torus["rotation"]);
		mat::Vec3 scale = LoadVec3(torus["scale"]);

		mat::UInt2 samples = LoadUInt2(torus["samples"]);
		float	smallRadius = torus["smallRadius"],
				largeRadius = torus["largeRadius"];
		ar::TorusDesc desc;
		desc.Samples = samples;
		desc.SmallRadius = smallRadius;
		desc.LargeRadius = largeRadius;

		auto entity = factory.CreateTorus(position, desc, id, name);

		auto& transform = entity.GetComponent<TransformComponent>();
		transform.Rotation = rotation;
		transform.AnglesRPY = transform.PreviousAnglesRPY = QuatToRPY(rotation);
		transform.Scale = transform.PreviousScale = scale;
		transform.DirtyFlag = true;

		return entity;
	}

	ar::Entity SceneImporter::ImportChain(json chain, SceneFactory& factory)
	{
		uint32_t id = chain["id"];
		std::string name = (chain.contains("name")) ? chain["name"] : "Chain";
		auto points = LoadControlPoints(chain["controlPoints"]);
		return factory.CreateChain(points, id, name);
	}

	ar::Entity SceneImporter::ImportBezierC0(json bezierC0, SceneFactory& factory)
	{
		uint32_t id = bezierC0["id"];
		std::string name = (bezierC0.contains("name")) ? bezierC0["name"] : "BezierC0";
		auto points = LoadControlPoints(bezierC0["controlPoints"]);
		return factory.CreateCurveC0(points, id, name);
	}

	ar::Entity SceneImporter::ImportBezierC2(json bezierC2, SceneFactory& factory)
	{
		uint32_t id = bezierC2["id"];
		std::string name = (bezierC2.contains("name")) ? bezierC2["name"] : "BezierC2";
		auto points = LoadControlPoints(bezierC2["controlPoints"]);
		return factory.CreateCurveC2(points, id, name);
	}

	ar::Entity SceneImporter::ImportInterpolatedC2(json intC2, SceneFactory& factory)
	{
		uint32_t id = intC2["id"];
		std::string name = (intC2.contains("name")) ? intC2["name"] : "InterpolatedC2";
		auto points = LoadControlPoints(intC2["controlPoints"]);
		return factory.CreateInterpolatedC2(points, id, name);
	}

	ar::Entity SceneImporter::ImportSurfaceC0(json surface, SceneFactory& factory)
	{
		uint32_t id = surface["id"];
		std::string name = (surface.contains("name")) ? surface["name"] : "SurfaceC0";
		mat::UInt2 size = LoadUInt2(surface["size"]);
		mat::UInt2 samples = LoadUInt2(surface["samples"]);
		auto points = LoadControlPoints(surface["controlPoints"]);
	
		// todo: detect if cylinder
		SurfaceType type = ar::SurfaceUtils::IsSurfaceC0Cylinder(points, size) ? SurfaceType::CYLINDERC0 : SurfaceType::RECTANGLEC0;


		return factory.CreateSurface(points, type, size, samples, id, name);
	}

	ar::Entity SceneImporter::ImportSurfaceC2(json surface, SceneFactory& factory)
	{
		uint32_t id = surface["id"];
		std::string name = (surface.contains("name")) ? surface["name"] : "SurfaceC0";
		mat::UInt2 size = LoadUInt2(surface["size"]);
		mat::UInt2 samples = LoadUInt2(surface["samples"]);
		auto points = LoadControlPoints(surface["controlPoints"]);

		// todo: detect if cylinder
		SurfaceType type = ar::SurfaceUtils::IsSurfaceC2Cylinder(points, size) ? SurfaceType::CYLINDERC2 : SurfaceType::RECTANGLEC2;


		return factory.CreateSurface(points, type, size, samples, id, name);
	}

	ar::mat::Vec3 SceneImporter::LoadVec3(json data)
	{
		return {
			data["x"].get<float>(),
			data["y"].get<float>(),
			data["z"].get<float>()
		};
	}

	ar::mat::UInt2 SceneImporter::LoadUInt2(json data)
	{
		return { data["u"], data["v"] };
	}

	ar::mat::Quat SceneImporter::LoadQuat(json data)
	{
		return { data["w"], data["x"], data["y"], data["z"] };
	}

	bool SceneImporter::LoadJsonFromFile(const std::string& filepath, json& output)
	{
		try
		{
			std::ifstream file(filepath);
			if (!file.is_open())
			{
				AR_ERROR("Could not open file: {0}", filepath);
				return false;
			}

			file >> output;
			file.close();
			return true;
		}
		catch (const json::parse_error& e)
		{
			AR_ERROR("JSON parse error: {0}", e.what());
			return false;
		}
		catch (const std::exception& e)
		{
			AR_ERROR("Error loading JSON: {0}", e.what());
			return false;
		}
	}

	std::vector<Entity> SceneImporter::LoadControlPoints(json data)
	{
		std::vector<Entity> points;
		auto pointIds = std::vector<int>();
		for (auto& ref : data)
		{
			const auto pid = ref["id"].get<uint32_t>();
			auto it = m_PointMap.find(pid);
			AR_ASSERT(it != m_PointMap.end(), "Control point id not found: {}", pid);
			points.push_back(it->second);
		}
		return points;
	}

}