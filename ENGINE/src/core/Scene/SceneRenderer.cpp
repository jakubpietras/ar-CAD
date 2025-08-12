#include "arpch.h"
#include "SceneRenderer.h"
#include "core/Renderer/Shader.h"
#include "core/Scene/Entity.h"

namespace ar
{

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: m_Scene(scene)
	{
		m_PointsVA = Ref<VertexArray>(VertexArray::Create());
	}

	void SceneRenderer::RenderScene(Ref<PerspectiveCamera> camera, RenderPassType pass)
	{
		ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
		ar::RenderCommand::Clear();
		ar::RenderCommand::ToggleDepthTest(true);
		ar::RenderCommand::ToggleBlendColor(true);
		glPointSize(8.0f);
		auto& vpMat = camera->GetVP();
		if (pass == RenderPassType::MAIN)
			RenderGrid(vpMat);
		RenderMeshes(vpMat, pass);
		RenderLines(vpMat, pass);
		RenderPoints(vpMat, pass);
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
					pickingShader->SetUInt("u_EntityID", idComp.ID);
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
			std::vector<VertexPosition> verts{};
			verts.reserve(cp.Points.size());
			for (auto& point : cp.Points)
			{
				verts.push_back({ point.GetComponent<TransformComponent>().Translation });
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
					auto idComp = m_Scene->m_Registry.get<IDComponent>(entity);
					shader->SetUInt("u_EntityID", idComp.ID);
					break;
				}
			}

			auto ent = Entity{ entity, m_Scene.get()};

			if (ent.HasComponent<ChainTagComponent>())
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

		std::vector<VertexPosition> unselectedVerts;
		std::vector<VertexPosition> selectedVerts;

		auto view = m_Scene->m_Registry.view<TransformComponent, PointComponent>();
		for (const auto& [e, transform, pt] : view.each())
		{
			bool selected = m_Scene->m_Registry.any_of<SelectedTagComponent>(e);
			if (selected)
				selectedVerts.push_back({ transform.Translation });
			else
				unselectedVerts.push_back({ transform.Translation });

		}

		// todo: IDs in SELECTION pass type
		// Draw unselected
		if (!unselectedVerts.empty())
		{
			shader->SetVec3("u_Color", { 1.f, 1.f, 1.f });
			m_PointsVA->ClearBuffers();
			m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(unselectedVerts)));
			ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
		}

		// Draw selected
		if (!selectedVerts.empty())
		{
			shader->SetVec3("u_Color", Renderer::SELECTION_COLOR);
			m_PointsVA->ClearBuffers();
			m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(selectedVerts)));
			ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
		}
	}

}
