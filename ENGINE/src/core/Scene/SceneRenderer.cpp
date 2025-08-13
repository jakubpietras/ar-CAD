#include "arpch.h"
#include "SceneRenderer.h"
#include "core/Renderer/Shader.h"
#include "core/Scene/Entity.h"

namespace ar
{
	const float SceneRenderer::CURSOR_SIZE = 15.0f;

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: m_Scene(scene),
		m_PickingFB(std::shared_ptr<Framebuffer>(Framebuffer::Create({ 1920, 1080, 1, TextureFormat::R32 }))),
		m_MainFB(std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create({ 1920, 1080 }))),
		m_CursorModelMtx(mat::Identity())
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
	}

	void SceneRenderer::OnResize(mat::Vec2 newVP)
	{
		m_MainFB->Resize(static_cast<uint32_t>(newVP.x), static_cast<uint32_t>(newVP.y));
		m_PickingFB->Resize(static_cast<uint32_t>(newVP.x), static_cast<uint32_t>(newVP.y));
	}

	void SceneRenderer::RenderMain(const Ref<CameraController>& cameraController, mat::Vec3 cursorPos)
	{
		m_MainFB->Bind();

		ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
		ar::RenderCommand::Clear();
		ar::RenderCommand::ToggleDepthTest(true);
		ar::RenderCommand::ToggleBlendColor(true);
		glPointSize(8.0f);
		auto& vpMat = cameraController->GetCamera()->GetVP();
		RenderGrid(vpMat);
		RenderMeshes(vpMat, RenderPassType::MAIN);
		RenderLines(vpMat, RenderPassType::MAIN);
		RenderPoints(vpMat, RenderPassType::MAIN);
		RenderCursor(cameraController, cursorPos);

		m_MainFB->Unbind();
	}

	void SceneRenderer::RenderPicking(const Ref<CameraController>& cameraController)
	{
		m_PickingFB->Bind();

		ar::RenderCommand::Clear();
		ar::RenderCommand::ToggleBlendColor(false);
		GLuint clearValue = 0;
		glClearBufferuiv(GL_COLOR, 0, &clearValue);

		glPointSize(8.0f);
		auto& vpMat = cameraController->GetCamera()->GetVP();
		RenderMeshes(vpMat, RenderPassType::SELECTION);
		RenderLines(vpMat, RenderPassType::SELECTION);
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
		ar::Renderer::Submit(ar::Primitive::Line, shader, m_CursorMesh, 6);
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

	void SceneRenderer::RenderMeshes(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto view = m_Scene->m_Registry.view<MeshComponent, TransformComponent>(entt::exclude<PointComponent>);
		for (auto [entity, mc, tc] : view.each())
		{
			switch (pass)
			{
				case RenderPassType::MAIN:
				{
					auto& shader = mc.Shader;
					shader->SetMat4("u_VP", viewProjection);
					shader->SetMat4("u_Model", tc.ModelMatrix);
					bool isSelected = m_Scene->m_Registry.any_of<SelectedTagComponent>(entity);
					auto color = isSelected ? Renderer::SELECTION_COLOR : mc.PrimaryColor;
					shader->SetVec3("u_Color", color);
					ar::Renderer::Submit(mc);
					break;
				}
				case RenderPassType::SELECTION:
				{
					auto mainShader = mc.Shader;
					auto pickingShader = ar::ShaderLib::Get("Picking");
					pickingShader->SetMat4("u_VP", viewProjection);
					pickingShader->SetMat4("u_Model", tc.ModelMatrix);

					auto idComp = m_Scene->m_Registry.get<IDComponent>(entity);
					mc.Shader = pickingShader;
					ar::Renderer::Submit(mc);
					mc.Shader = mainShader;
					break;
				}
			}
		}
	}

	void SceneRenderer::RenderLines(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto shader = pass == RenderPassType::MAIN ? ShaderLib::Get("Basic") : ShaderLib::Get("Picking");
		shader->SetMat4("u_VP", viewProjection);
		shader->SetMat4("u_Model", mat::Identity());

		auto view = m_Scene->m_Registry.view<ControlPointsComponent>();
		for (auto [entity, cp] : view.each())
		{
			auto parentEntity = ar::Entity(entity, m_Scene.get());
			std::vector<VertexPositionID> verts{};
			verts.reserve(cp.Points.size());
			for (auto& point : cp.Points)
			{
				verts.push_back({ point.GetComponent<TransformComponent>().Translation, parentEntity.GetID()});
			}

			switch (pass)
			{
				case RenderPassType::MAIN:
				{
					auto color = m_Scene->m_Registry.any_of<SelectedTagComponent>(entity) ?
						Renderer::SELECTION_COLOR : ar::mat::Vec3(1.f, 1.f, 1.f);
					shader->SetVec3("u_Color", color);
					break;
				}
				case RenderPassType::SELECTION:
				{
					break;
				}
			}
			if (parentEntity.HasComponent<ChainTagComponent>())
			{
				m_PointsVA->ClearBuffers();
				m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(verts)));
				ar::Renderer::Submit(Primitive::LineStrip, shader, m_PointsVA);
			}
		}
	}

	void SceneRenderer::RenderPoints(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto shader = pass == RenderPassType::MAIN ? ShaderLib::Get("Basic") : ShaderLib::Get("Picking");
		shader->SetMat4("u_VP", viewProjection);
		shader->SetMat4("u_Model", mat::Identity());

		std::vector<VertexPositionID> unselectedVerts, selectedVerts;

		auto view = m_Scene->m_Registry.view<TransformComponent, PointComponent>();
		for (const auto& [e, transform, pt] : view.each())
		{
			auto entity = ar::Entity(e, m_Scene.get());
			bool selected = m_Scene->m_Registry.any_of<SelectedTagComponent>(e);
			if (selected)
				selectedVerts.push_back({ transform.Translation, entity.GetID() });
			else
				unselectedVerts.push_back({ transform.Translation, entity.GetID() });
		}

		switch (pass)
		{
			case RenderPassType::MAIN:
			{
				if (!unselectedVerts.empty())
				{
					shader->SetVec3("u_Color", { 1.f, 1.f, 1.f });
					m_PointsVA->ClearBuffers();
					m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(unselectedVerts)));
					ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
				}

				if (!selectedVerts.empty())
				{
					shader->SetVec3("u_Color", Renderer::SELECTION_COLOR);
					m_PointsVA->ClearBuffers();
					m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(selectedVerts)));
					ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
				}
				break;
			}
			case RenderPassType::SELECTION:
			{
				std::vector<VertexPositionID> allVerts;
				allVerts.reserve(unselectedVerts.size() + selectedVerts.size());
				allVerts.insert(allVerts.end(), unselectedVerts.begin(), unselectedVerts.end());
				allVerts.insert(allVerts.end(), selectedVerts.begin(), selectedVerts.end());

				m_PointsVA->ClearBuffers();
				m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(allVerts)));
				ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
				break;
			}
			default:
				return;
		}
	}

}
