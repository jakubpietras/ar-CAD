#pragma once
#include "core/Core.h"
#include "core/Scene/Scene.h"
#include "core/Scene/Entity.h"

namespace ar
{
	class SceneFactory
	{
	public:
		SceneFactory(Ref<Scene> scene);

		Entity CreatePoint(mat::Vec3 spawnPoint);
		Entity CreateTorus(mat::Vec3 spawnPoint, TorusDesc desc);
		Entity CreateChain(std::vector<Entity> points);
		Entity CreateCurveC0(std::vector<Entity> points);
		Entity CreateCurveC2(std::vector<Entity> points);
		Entity CreateInterpolatedC2(std::vector<Entity> points);
		Entity CreateSurface(mat::Vec3 origin, SurfaceDesc desc);
		Entity CreateSurface(mat::Vec3 origin, SurfaceDesc desc, ar::Entity tempSurface);

	private:
		Ref<Scene> m_Scene;
	};
}

