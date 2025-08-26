#pragma once
#include "core/Core.h"
#include "core/Scene/Scene.h"
#include "core/Scene/Entity.h"
#include <optional>

namespace ar
{
	class SceneFactory
	{
	public:
		SceneFactory(Ref<Scene> scene);

		Entity CreatePoint(mat::Vec3 spawnPoint, std::optional<uint32_t> id, const std::string& name = std::string());
		Entity CreateTorus(mat::Vec3 spawnPoint, TorusDesc desc, std::optional<uint32_t> id, const std::string& name = std::string());
		Entity CreateChain(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name = std::string());
		Entity CreateCurveC0(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name = std::string());
		Entity CreateCurveC2(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name = std::string());
		Entity CreateInterpolatedC2(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name = std::string());
		Entity CreateSurface(std::vector<Entity>& points, bool isC0, mat::UInt2 size, mat::UInt2 samples, std::optional<uint32_t> id, const std::string& name = std::string());
		Entity CreateSurface(mat::Vec3 origin, SurfaceDesc desc, ar::Entity tempSurface);

	private:
		Ref<Scene> m_Scene;
	};
}

