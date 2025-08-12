#pragma once
#include "core/Core.h"
#include "core/Scene/Scene.h"
#include "core/Renderer/VertexArray.h"
#include "core/Renderer/Camera.h"
#include "core/Renderer/Renderer.h"

namespace ar
{

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> scene);
		void RenderScene(Ref<PerspectiveCamera> camera, RenderPassType pass);
		void RenderGrid(ar::mat::Mat4 viewProjection);
		void RenderMeshes(ar::mat::Mat4 viewProjection, RenderPassType pass);
		void RenderLines(ar::mat::Mat4 viewProjection, RenderPassType pass);
		void RenderPoints(ar::mat::Mat4 viewProjection, RenderPassType pass);

	private:
		Ref<Scene> m_Scene;
		Ref<VertexArray> m_PointsVA;
	};

}

