#include "arpch.h"
#include "SceneExporter.h"
#include "core/Scene/Components.h"

namespace ar
{

	bool SceneExporter::Export(const std::string& filepath, Ref<Scene> scene)
	{
		json output;
		output["points"] = json::array();
		output["geometry"] = json::array();

		auto points = scene->m_Registry.view<PointComponent>();
		for (auto& point : points)
			output["points"].push_back(SerializePoint({ point, scene.get() }));

		auto tori = scene->m_Registry.view<TorusComponent>();
		for (auto& torus : tori)
			output["geometry"].push_back(SerializeTorus({ torus, scene.get() }));

		// curves
		auto chains = scene->m_Registry.view<ChainComponent>();
		for (auto& chain : chains)
			output["geometry"].push_back(SerializeCurve({ chain, scene.get() }, "chain"));
		auto bezierCurvesC0 = scene->m_Registry.view<CurveC0Component>();
		for (auto& bcurvec0 : bezierCurvesC0)
			output["geometry"].push_back(SerializeCurve({ bcurvec0, scene.get() }, "bezierC0"));
		auto bezierCurvesC2 = scene->m_Registry.view<CurveC2Component>();
		for (auto& bcurvec2 : bezierCurvesC2)
			output["geometry"].push_back(SerializeCurve({ bcurvec2, scene.get() }, "bezierC2"));
		auto intC2 = scene->m_Registry.view<InterpolatedC2Component>();
		for (auto& intcurve : intC2)
			output["geometry"].push_back(SerializeCurve({ intcurve, scene.get() }, "interpolatedC2"));

		// surfaces
		auto surfaces = scene->m_Registry.view<SurfaceComponent>();
		for (auto& surface : surfaces)
		{
			auto entity = Entity(surface, scene.get());
			auto& sd = entity.GetComponent<SurfaceComponent>();
			if (sd.Description.Type == SurfaceType::RECTANGLEC0 || sd.Description.Type == SurfaceType::CYLINDERC0)
				output["geometry"].push_back(SerializeSurface(entity, "bezierSurfaceC0"));
			else
				output["geometry"].push_back(SerializeSurface(entity, "bezierSurfaceC2"));
		}


		auto ret = SaveJson(output, filepath);
		if (!ret)
		{
			AR_ERROR("Exporting scene failed, cannot write to {0}", filepath);
			return false;
		}
		return true;
	}

	json SceneExporter::SerializePoint(Entity point)
	{
		json result;
		result["id"] = point.GetID();
		result["name"] = point.GetName();
		result["position"] = SerializeVec3(point.GetComponent<TransformComponent>().Translation);
		return result;
	}

	json SceneExporter::SerializeTorus(Entity torus)
	{
		json result;
		result["id"] = torus.GetID();
		result["name"] = torus.GetName();
		result["objectType"] = "torus";
		
		auto& transform = torus.GetComponent<TransformComponent>();
		result["position"] = SerializeVec3(transform.Translation);
		result["rotation"] = SerializeQuat(transform.Rotation);
		result["scale"] = SerializeVec3(transform.Scale);
		
		auto& t = torus.GetComponent<TorusComponent>();
		result["samples"] = SerializeUInt2(t.Description.Samples);
		result["smallRadius"] = t.Description.SmallRadius;
		result["largeRadius"] = t.Description.LargeRadius;

		return result;
	}

	json SceneExporter::SerializeCurve(Entity curve, const std::string& objectType)
	{
		json result;
		result["objectType"] = objectType;
		result["id"] = curve.GetID();
		result["name"] = curve.GetName();
		result["controlPoints"] = SerializeControlPoints(curve);
		return result;
	}

	json SceneExporter::SerializeSurface(Entity surface, const std::string& objectType)
	{
		json result;
		result["objectType"] = objectType;
		result["id"] = surface.GetID();
		result["name"] = surface.GetName();
		result["controlPoints"] = SerializeControlPoints(surface);
		
		auto& s = surface.GetComponent<SurfaceComponent>();
		result["size"] = SerializeUInt2(s.Description.Size);
		result["samples"] = SerializeUInt2(s.Description.Samples);
		
		return result;
	}

	json SceneExporter::SerializeVec3(mat::Vec3 v)
	{
		json output;
		output["x"] = v.x;
		output["y"] = v.y;
		output["z"] = v.z;
		return output;
	}

	json SceneExporter::SerializeQuat(mat::Quat q)
	{
		json output;
		output["x"] = q.x;
		output["y"] = q.y;
		output["z"] = q.z;
		output["w"] = q.w;
		return output;
	}

	json SceneExporter::SerializeUInt2(mat::UInt2 v)
	{
		json output;
		output["u"] = v.u;
		output["v"] = v.v;
		return output;
	}

	json SceneExporter::SerializeControlPoints(Entity parent)
	{
		json points = json::array();
		auto& cp = parent.GetComponent<ControlPointsComponent>().Points;
		for (auto& point : cp)
		{
			json ref;
			ref["id"] = point.GetID();
			points.push_back(ref);
		}
		return points;
	}

	bool SceneExporter::SaveJson(json data, const std::string& filepath)
	{
		std::ofstream f(filepath);
		if (!f.is_open())
			return false;

		f << data.dump(4);
		f.close();
		return true;
	}

}