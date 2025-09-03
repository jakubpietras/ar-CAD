#include "arpch.h"
#include "SceneRenderer.h"
#include "core/Renderer/Shader.h"
#include "core/Scene/Entity.h"
#include "core/Scene/Components.h"

namespace ar
{
	const float SceneRenderer::CURSOR_SIZE = 15.0f;
	const float SceneRenderer::MEAN_POINT_SIZE = 7.0f;
	const mat::Vec3 SceneRenderer::SELECTION_COLOR = { .702f, .302f, 1.f };
	const mat::Vec3 SceneRenderer::POLYGON_COLOR = { .702f, .302f, 1.f };
	const mat::Vec3 SceneRenderer::MIDDLE_POINT_COLOR = { .702f, .302f, 1.f };

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: m_Scene(scene),
		m_PickingFB(std::shared_ptr<Framebuffer>(Framebuffer::Create({ 1920, 1080, 1, TextureFormat::R32 }))),
		m_MainFB(std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create({ 1920, 1080 }))),
		m_CursorModelMtx(mat::Identity()), m_MeanPointModelMtx(mat::Identity())
	{
		m_PointsVA = Ref<VertexArray>(VertexArray::Create());

		m_CursorMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		std::vector<ar::VertexPositionColor> vertices{
			{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
		};
		m_CursorMesh->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(vertices)));

		m_MeanPointMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		std::vector<ar::VertexPositionColor> verts{
			{{1.0f, 0.0f, 0.0f}, MIDDLE_POINT_COLOR},
			{{-1.0f, 0.0f, 0.0f}, MIDDLE_POINT_COLOR},
			{{0.0f, 1.0f, 0.0f}, MIDDLE_POINT_COLOR},
			{{0.0f, -1.0f, 0.0f}, MIDDLE_POINT_COLOR},
			{{0.0f, 0.0f, 1.0f}, MIDDLE_POINT_COLOR},
			{{0.0f, 0.0f, -1.0f}, MIDDLE_POINT_COLOR}
		};
		m_MeanPointMesh->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(verts)));
	}

	void SceneRenderer::OnResize(mat::Vec2 newVP)
	{
		m_MainFB->Resize(static_cast<uint32_t>(newVP.x), static_cast<uint32_t>(newVP.y));
		m_PickingFB->Resize(static_cast<uint32_t>(newVP.x), static_cast<uint32_t>(newVP.y));
	}

	void SceneRenderer::RenderMain(const Ref<CameraController>& cameraController, mat::Vec2 viewport, mat::Vec3 cursorPos, mat::Vec3 meanPointPos, bool renderMeanPoint)
	{
		m_MainFB->Bind();

		ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
		ar::RenderCommand::Clear();
		ar::RenderCommand::ToggleDepthTest(true);
		ar::RenderCommand::ToggleBlendColor(true);
		glPointSize(8.0f);
		auto& vpMat = cameraController->GetCamera()->GetVP();
		RenderGrid(vpMat);
		RenderMeshes(vpMat, RenderPassType::MAIN, viewport);
		RenderSurfaces(vpMat, RenderPassType::MAIN);
		RenderGregoryPatches(vpMat, RenderPassType::MAIN);
		if (renderMeanPoint)
			RenderMeanPoint(cameraController, meanPointPos);
		RenderCursor(cameraController, cursorPos);
		//RenderPolygons(vpMat);
		RenderPoints(vpMat, RenderPassType::MAIN);
		m_MainFB->Unbind();
	}

	void SceneRenderer::RenderPicking(const Ref<CameraController>& cameraController, mat::Vec2 viewport)
	{
		m_PickingFB->Bind();

		ar::RenderCommand::Clear();
		ar::RenderCommand::ToggleBlendColor(false);
		GLuint clearValue = 0;
		glClearBufferuiv(GL_COLOR, 0, &clearValue);

		glPointSize(8.0f);
		auto& vpMat = cameraController->GetCamera()->GetVP();
		RenderMeshes(vpMat, RenderPassType::SELECTION, viewport);
		RenderSurfaces(vpMat, RenderPassType::SELECTION);
		RenderGregoryPatches(vpMat, RenderPassType::SELECTION);
		RenderPoints(vpMat, RenderPassType::SELECTION);

		m_PickingFB->Unbind();
	}

	std::unordered_set<uint32_t> SceneRenderer::ReadPixels(ar::mat::Vec2 boxStart, ar::mat::Vec2 boxEnd)
	{
		auto x = static_cast<int>(std::min(boxStart.x, boxEnd.x));
		auto y = static_cast<int>(std::min(boxStart.y, boxEnd.y));
		auto w = static_cast<int>(std::abs(boxEnd.x - boxStart.x));
		auto h = static_cast<int>(std::abs(boxEnd.y - boxStart.y));
		int flippedY = m_PickingFB->GetHeight() - y - h;
		// if LMB is pressed and released in the same spot, then we want to read one pixel (not 0)
		w = std::max(w, 1);
		h = std::max(h, 1);
		
		std::vector<uint32_t> pixels(w * h);

		m_PickingFB->Bind();
		glReadPixels(x, flippedY, w, h, GL_RED_INTEGER, GL_UNSIGNED_INT, pixels.data());
		m_PickingFB->Unbind();

		std::unordered_set<uint32_t> uniqueIDs;
		for (auto id : pixels)
		{
			if (id != 0)
				uniqueIDs.insert(id);
		}
		return uniqueIDs;
	}

	void SceneRenderer::RenderMeanPoint(ar::Ref<ar::CameraController> camera, ar::mat::Vec3 position)
	{
		auto positionViewSpace = camera->GetCamera()->GetView() * ar::mat::Vec4(position, 1.0f);
		float depth = std::abs(positionViewSpace.z);
		float pixelSizeAtDepth = 2.0f * depth * tan(camera->GetFOV() * 0.5f) / m_MainFB->GetHeight();
		float scale = pixelSizeAtDepth * MEAN_POINT_SIZE;

		m_MeanPointModelMtx =
			ar::mat::TranslationMatrix(position)
			* ar::mat::ScaleMatrix({ scale, scale, scale });

		auto shader = ar::ShaderLib::Get("BasicColor");
		shader->SetMat4("u_Model", m_MeanPointModelMtx);
		shader->SetMat4("u_VP", camera->GetCamera()->GetVP());

		ar::RenderCommand::SetLineThickness(3);
		ar::RenderCommand::ToggleDepthTest(false);
		ar::Renderer::Submit(ar::Primitive::Line, shader, m_MeanPointMesh, false);
		ar::RenderCommand::ToggleDepthTest(false);
		ar::RenderCommand::SetLineThickness();
	}

	void SceneRenderer::RenderCursor(ar::Ref<ar::CameraController> camera, ar::mat::Vec3 position)
	{
		auto positionViewSpace = camera->GetCamera()->GetView() * ar::mat::Vec4(position, 1.0f);
		float depth = std::abs(positionViewSpace.z);
		float pixelSizeAtDepth = 2.0f * depth * tan(camera->GetFOV() * 0.5f) / m_MainFB->GetHeight();
		float scale = pixelSizeAtDepth * CURSOR_SIZE;

		m_CursorModelMtx =
			ar::mat::TranslationMatrix(position)
			* ar::mat::ScaleMatrix({ scale, scale, scale });

		auto shader = ar::ShaderLib::Get("BasicColor");
		shader->SetMat4("u_Model", m_CursorModelMtx);
		shader->SetMat4("u_VP", camera->GetCamera()->GetVP());

		ar::RenderCommand::SetLineThickness(3);
		ar::RenderCommand::ToggleDepthTest(false);
		ar::Renderer::Submit(ar::Primitive::Line, shader, m_CursorMesh, false, 1);
		ar::RenderCommand::ToggleDepthTest(false);
		ar::RenderCommand::SetLineThickness();
	}

	void SceneRenderer::RenderGrid(ar::mat::Mat4 viewProjection)
	{
		auto gridShader = ar::ShaderLib::Get("Grid");
		ar::RenderCommand::ToggleDepthTest(false);
		gridShader->SetMat4("u_VP", viewProjection);
		ar::Renderer::Submit(ar::Primitive::Triangle, gridShader, 6);
		ar::RenderCommand::ToggleDepthTest(true);
	}

	void SceneRenderer::RenderMeshes(ar::mat::Mat4 viewProjection, RenderPassType pass, ar::mat::Vec2 viewport)
	{
		auto view = m_Scene->m_Registry.view<MeshComponent>(entt::exclude<PointComponent, HiddenMeshTagComponent, SurfaceComponent, GregoryPatchComponent>);
		for (auto [entity, mc] : view.each())
		{
			auto e = ar::Entity(entity, m_Scene.get());
			auto hasTransform = e.HasComponent<TransformComponent>();
			auto model = hasTransform ? e.GetComponent<TransformComponent>().ModelMatrix : mat::Identity();

			switch (pass)
			{
				case RenderPassType::MAIN:
				{
					mc.ShaderUsed = ShaderType::MAIN;
					auto shader = mc.GetShader();
					shader->SetMat4("u_VP", viewProjection);
					shader->SetMat4("u_Model", model);
					if (mc.AdaptiveDrawing)
						shader->SetVec2("u_Viewport", viewport.Data());

					bool isSelected = m_Scene->m_Registry.any_of<SelectedTagComponent>(entity);
					auto color = isSelected ? Renderer::SELECTION_COLOR : mc.PrimaryColor;
					shader->SetVec3("u_Color", color);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());
					break;
				}
				case RenderPassType::SELECTION:
				{
					mc.ShaderUsed = ShaderType::PICKING;
					auto pickingShader = mc.GetShader();
					pickingShader->SetMat4("u_VP", viewProjection);
					pickingShader->SetMat4("u_Model", model);
					if (mc.AdaptiveDrawing)
						pickingShader->SetVec2("u_Viewport", viewport.Data());
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());
					break;
				}
			}
		}
	}

	void SceneRenderer::RenderSurfaces(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto view = m_Scene->m_Registry.view<MeshComponent, SurfaceComponent>(entt::exclude<HiddenMeshTagComponent>);
		for (auto [entity, mc, bs] : view.each())
		{
			auto model = mat::Identity();
			switch (pass)
			{
				case RenderPassType::MAIN:
				{
					mc.ShaderUsed = ShaderType::MAIN;
					auto shader = mc.GetShader();
					shader->SetMat4("u_VP", viewProjection);
					shader->SetMat4("u_Model", model);
					shader->SetUInt("u_SamplesU", bs.Description.Samples.u);
					shader->SetUInt("u_SamplesV", bs.Description.Samples.v);

					bool isSelected = m_Scene->m_Registry.any_of<SelectedTagComponent>(entity);
					auto color = isSelected ? Renderer::SELECTION_COLOR : mc.PrimaryColor;
					shader->SetVec3("u_Color", color);

					shader->SetBool("u_SwitchCoords", false);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());
					shader->SetBool("u_SwitchCoords", true);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());

					break;
				}
				case RenderPassType::SELECTION:
				{
					mc.ShaderUsed = ShaderType::PICKING;
					auto pickingShader = mc.GetShader();
					pickingShader->SetMat4("u_VP", viewProjection);
					pickingShader->SetMat4("u_Model", model);
					pickingShader->SetUInt("u_SamplesU", bs.Description.Samples.u);
					pickingShader->SetUInt("u_SamplesV", bs.Description.Samples.v);

					pickingShader->SetBool("u_SwitchCoords", false);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());
					pickingShader->SetBool("u_SwitchCoords", true);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());

					break;
				}
			}
		}
	}

	void SceneRenderer::RenderPolygons(ar::mat::Mat4 viewProjection)
	{
		// Render Bezier polygons
		auto c0curves = m_Scene->m_Registry.view<CurveC0Component, MeshComponent>();
		auto c2curves = m_Scene->m_Registry.view<CurveC2Component, MeshComponent>();

		auto shader = ShaderLib::Get("Basic");
		shader->SetMat4("u_VP", viewProjection);
		shader->SetMat4("u_Model", mat::Identity());
		shader->SetVec3("u_Color", POLYGON_COLOR);

		for (const auto& [entity, cc, mc] : c0curves.each())
		{
			if (cc.ShowPolygon)
				ar::Renderer::Submit(Primitive::LineStrip, shader, mc.VertexArray, false);
		}
		for (const auto& [entity, cc, mc] : c2curves.each())
		{
			if (cc.ShowPolygon)
				ar::Renderer::Submit(Primitive::LineStrip, shader, mc.VertexArray, false);
		}

		auto surfaces = m_Scene->m_Registry.view<SurfaceComponent, ControlMeshComponent>();
		for (const auto& [entity, sc, cmc] : surfaces.each())
		{
			if (sc.ShowNet)
				ar::Renderer::Submit(Primitive::Line, cmc.Shader, cmc.VertexArray, true);
		}
	}

	void SceneRenderer::RenderPoints(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto shader = pass == RenderPassType::MAIN ? ShaderLib::Get("Point") : ShaderLib::Get("PointPicking");
		shader->SetMat4("u_VP", viewProjection);
		shader->SetMat4("u_Model", mat::Identity());

		auto view = m_Scene->m_Registry.view<TransformComponent, PointComponent>();
		if (!view.size_hint())
			return;

		bool regeneratePoints = true;

		/*for (const auto& [e, transform, pt] : view.each())
		{
			if (transform.DirtyFlag)
			{
				regeneratePoints = true;
				break;
			}
		}*/

		if (regeneratePoints)
		{
			std::vector<VertexPositionIDColor> pointVerts;
			for (const auto& [e, transform, pt] : view.each())
			{
				auto entity = ar::Entity(e, m_Scene.get());
				bool selected = m_Scene->m_Registry.any_of<SelectedTagComponent>(e);
				if (selected)
				{
					mat::Vec3 color = pt.ShouldUseTempColor ? pt.TempColor : Renderer::SELECTION_COLOR;
					pointVerts.push_back({ transform.Translation, entity.GetID(), color });
				}
				else
				{
					mat::Vec3 color = pt.ShouldUseTempColor ? pt.TempColor : pt.Color;
					pointVerts.push_back({ transform.Translation, entity.GetID(), color });
				}
			}
			if (!pointVerts.empty())
			{
				m_PointsVA->ClearBuffers();
				m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(pointVerts)));
			}
		}

		ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
	}

	void SceneRenderer::RenderGregoryPatches(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto view = m_Scene->m_Registry.view<MeshComponent, GregoryPatchComponent>(entt::exclude<HiddenMeshTagComponent>);
		for (auto [entity, mc, gp] : view.each())
		{
			auto model = mat::Identity();
			switch (pass)
			{
				case RenderPassType::MAIN:
				{
					mc.ShaderUsed = ShaderType::MAIN;
					auto shader = mc.GetShader();
					shader->SetMat4("u_VP", viewProjection);
					shader->SetMat4("u_Model", model);
					shader->SetUInt("u_SamplesU", gp.Samples.u);
					shader->SetUInt("u_SamplesV", gp.Samples.v);

					bool isSelected = m_Scene->m_Registry.any_of<SelectedTagComponent>(entity);
					auto color = isSelected ? Renderer::SELECTION_COLOR : mc.PrimaryColor;
					shader->SetVec3("u_Color", color);

					shader->SetBool("u_SwitchCoords", false);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());
					shader->SetBool("u_SwitchCoords", true);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());

					break;
				}
				case RenderPassType::SELECTION:
				{
					mc.ShaderUsed = ShaderType::PICKING;
					auto pickingShader = mc.GetShader();
					pickingShader->SetMat4("u_VP", viewProjection);
					pickingShader->SetMat4("u_Model", model);
					pickingShader->SetUInt("u_SamplesU", gp.Samples.u);
					pickingShader->SetUInt("u_SamplesV", gp.Samples.v);

					pickingShader->SetBool("u_SwitchCoords", false);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());
					pickingShader->SetBool("u_SwitchCoords", true);
					ar::Renderer::Submit(mc, mc.VertexArray->IsIndexed());

					break;
				}
			}
		}
	}

}
