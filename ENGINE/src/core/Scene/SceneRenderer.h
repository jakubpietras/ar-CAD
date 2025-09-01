#pragma once
#include "core/Core.h"
#include "core/Scene/Scene.h"
#include "core/Renderer/VertexArray.h"
#include "core/CameraController.h"
#include "core/Renderer/Renderer.h"
#include <unordered_set>

namespace ar
{

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> scene);

		void OnResize(mat::Vec2 newVP);

		void RenderMain(const Ref<CameraController>& cameraController, mat::Vec2 viewport, mat::Vec3 cursorPos, mat::Vec3 meanPointPos, bool renderMeanPoint);
		void RenderPicking(const Ref<CameraController>& cameraController, mat::Vec2 viewport);

		std::unordered_set<uint32_t> ReadPixels(ar::mat::Vec2 boxStart, ar::mat::Vec2 boxEnd);

		Ref<Framebuffer> GetMainFramebuffer() const { return m_MainFB; }
		Ref<Framebuffer> GetPickingFramebuffer() const { return m_PickingFB; }

	private:
		static const float CURSOR_SIZE;
		static const float MEAN_POINT_SIZE;

		static const mat::Vec3 POLYGON_COLOR;
		static const mat::Vec3 MIDDLE_POINT_COLOR;
		static const mat::Vec3 SELECTION_COLOR;

		Ref<Scene> m_Scene;
		
		Ref<VertexArray> m_PointsVA;
		Ref<VertexArray> m_CursorMesh, m_MeanPointMesh;
		ar::mat::Mat4 m_CursorModelMtx, m_MeanPointModelMtx;

		Ref<Framebuffer> m_MainFB;
		Ref<Framebuffer> m_PickingFB;
	
		void RenderMeanPoint(ar::Ref<ar::CameraController> cameraController, ar::mat::Vec3 position);
		void RenderCursor(ar::Ref<ar::CameraController> cameraController, ar::mat::Vec3 position);
		void RenderGrid(ar::mat::Mat4 viewProjection);
		void RenderMeshes(ar::mat::Mat4 viewProjection, RenderPassType pass, ar::mat::Vec2 viewport);
		void RenderSurfaces(ar::mat::Mat4 viewProjection, RenderPassType pass);
		void RenderPolygons(ar::mat::Mat4 viewProjection);
		void RenderPoints(ar::mat::Mat4 viewProjection, RenderPassType pass);
		
	};

}

