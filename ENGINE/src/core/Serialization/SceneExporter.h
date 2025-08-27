#pragma once
#include "core/Core.h"
#include <nlohmann/json.hpp>
#include "core/Scene/Scene.h"
#include "core/Scene/Entity.h"

using json = nlohmann::ordered_json;

namespace ar
{
	class SceneExporter
	{
	public:
		static bool Export(const std::string& filepath, Ref<Scene> scene);
	private:


		static json SerializePoint(Entity point);
		static json SerializeTorus(Entity torus);
		static json SerializeCurve(Entity curve, const std::string& objectType);
		static json SerializeSurface(Entity surface, const std::string& objectType);
		static json SerializeControlPoints(Entity parent);

		static json SerializeVec3(mat::Vec3 v);
		static json SerializeQuat(mat::Quat q);
		static json SerializeUInt2(mat::UInt2 v);

		static bool SaveJson(json data, const std::string& filepath);
	};
}