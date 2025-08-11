#include "arpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "core/UID.h"

namespace ar
{

	Scene::Scene()
	{
		AR_INFO("Scene initialized!");
		m_PointsVA = Ref<VertexArray>(VertexArray::Create());
	}

	Scene::~Scene() { }

	Entity Scene::CreateEntity(const std::string& name /*= std::string()*/)
	{
		Entity entity = { m_Registry.create(), this };
		auto& ic = entity.AddComponent<IDComponent>();
		ic.ID = UID::AssignNew();
		auto& tc = entity.AddComponent<TagComponent>();
		tc.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[ic.ID] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetID());
		m_Registry.destroy(entity);
	}

	Entity Scene::FindEntityByName(const std::string& name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (const auto& [entity, tagComp] : view.each())
		{
			if (tagComp.Tag == name)
				return Entity{ entity, this };
		}
		return {};
	}

	Entity Scene::GetEntityByID(uint32_t id)
	{
		if (m_EntityMap.find(id) != m_EntityMap.end())
			return { m_EntityMap.at(id), this };
		return {};
	}

	void Scene::OnUpdate(Ref<PerspectiveCamera> camera)
	{
		UpdateScene();
		RenderScene(camera);
	}

	void Scene::UpdateScene()
	{
		auto transformView = m_Registry.view<TransformComponent>();
		for (auto [entity, tc] : transformView.each())
		{
			UpdateTransform(tc);
		}

		auto torusView = m_Registry.view<TorusComponent, MeshComponent>();
		for (auto [entity, toc, mc] : torusView.each())
		{
			if (!toc.DirtyFlag)
				continue;
			mc.VertexArray->ClearBuffers();
			toc.Vertices = ar::TorusUtils::GenerateTorusVertices(toc.Description);
			toc.Edges = ar::TorusUtils::GenerateTorusEdges(toc.Description);
			mc.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(toc.Vertices)));
			auto indexBuffers = ar::IndexBuffer::Create(toc.Edges);
			for (auto& ib : indexBuffers)
				mc.VertexArray->AddIndexBuffer(ib);
			toc.DirtyFlag = false;
		}

	}

	void Scene::UpdateTransform(TransformComponent& tc)
	{
		if (!tc.DirtyFlag)
			return;

		// ---- Scale ----
		auto v = tc.Translation - tc.PivotPoint;
		auto d_scale = tc.Scale / tc.PreviousScale;
		auto scaled_v = d_scale * v;
		tc.Translation = tc.PivotPoint + scaled_v;

		// ---- Rotation ----
		v = tc.Translation - tc.PivotPoint;
		auto q = mat::RPYToQuat(tc.AnglesRPY - tc.PreviousAnglesRPY);
		auto rotated_v = q * mat::Quat(0.0f, v.x, v.y, v.z) * mat::Conjugate(q);
		tc.Translation = tc.PivotPoint + QuatToVec3(rotated_v);
		tc.Rotation = mat::Normalize(q * tc.Rotation);

		// ---- Combination ----
		tc.ModelMatrix =
			mat::TranslationMatrix(tc.Translation) *
			mat::ToMat4(tc.Rotation) *
			mat::ScaleMatrix(tc.Scale);

		tc.PivotPoint = tc.Translation;

		tc.DirtyFlag = false;
	}

	void Scene::RenderScene(Ref<PerspectiveCamera> camera)
	{
		ar::Renderer::BeginScene();

		auto& vpMat = camera->GetVP();
		RenderGrid(vpMat);
		RenderMeshes(vpMat, RenderPassType::MAIN);
		RenderLines(vpMat, RenderPassType::MAIN);
		RenderPoints(vpMat, RenderPassType::MAIN);

		ar::Renderer::EndScene();
	}


	void Scene::RenderGrid(ar::mat::Mat4 viewProjection)
	{
		auto gridShader = ar::ShaderLib::Get("Grid");
		ar::RenderCommand::ToggleDepthTest(false);
		gridShader->SetMat4("u_VP", viewProjection);
		ar::Renderer::Submit(ar::Primitive::Triangle, gridShader, 6);
		ar::RenderCommand::ToggleDepthTest(true);
	}

	void Scene::RenderMeshes(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto view = m_Registry.view<MeshComponent, TransformComponent>(entt::exclude<PointComponent>);
		for (auto [entity, mc, tc] : view.each())
		{
			mc.Shader->SetMat4("u_VP", viewProjection);
			mc.Shader->SetMat4("u_Model", tc.ModelMatrix);

			switch (pass)
			{
				case RenderPassType::MAIN:
				{
					bool isSelected = m_Registry.any_of<SelectedTagComponent>(entity);
					auto color = isSelected ? Renderer::SELECTION_COLOR : mc.PrimaryColor;
					mc.Shader->SetVec3("u_Color", color);
					break;
				}
			}
			ar::Renderer::Submit(mc);
		}
	}

	void Scene::RenderLines(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto shader = ShaderLib::Get("Basic");
		shader->SetMat4("u_VP", viewProjection);
		shader->SetMat4("u_Model", mat::Identity());

		auto view = m_Registry.view<ControlPointsComponent>();
		for (auto [entity, cp] : view.each())
		{
			std::vector<VertexPosition> verts{};
			verts.reserve(cp.Points.size());
			for (auto& point : cp.Points)
			{
				verts.push_back({ point.GetComponent<TransformComponent>().Translation });
			}

			ar::mat::Vec3 color{1.f, 1.f, 1.f};
			switch (pass)
			{
				case RenderPassType::MAIN:
				{
					color = m_Registry.any_of<SelectedTagComponent>(entity) ? 
						Renderer::SELECTION_COLOR : ar::mat::Vec3(1.f, 1.f, 1.f);
					break;
				}
			}
			shader->SetVec3("u_Color", color);

			auto ent = ar::Entity{ entity, this };

			if (ent.HasComponent<ChainTagComponent>())
			{
				m_PointsVA->ClearBuffers();
				m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(verts)));
				ar::Renderer::Submit(Primitive::LineStrip, shader, m_PointsVA);
			}
		}
	}

	void Scene::RenderPoints(ar::mat::Mat4 viewProjection, RenderPassType pass)
	{
		auto shader = ShaderLib::Get("Basic");
		shader->SetMat4("u_VP", viewProjection);
		shader->SetMat4("u_Model", mat::Identity());

		std::vector<VertexPosition> unselectedVerts;
		std::vector<VertexPosition> selectedVerts;

		auto view = m_Registry.view<TransformComponent, PointComponent>();
		for (const auto& [e, transform, pt] : view.each())
		{
			bool selected = m_Registry.any_of<SelectedTagComponent>(e);
			if (selected)
				selectedVerts.push_back({ transform.Translation });
			else
				unselectedVerts.push_back({ transform.Translation });
			
		}

		// Draw unselected
		if (!unselectedVerts.empty())
		{
			shader->SetVec3("u_Color", {1.f, 1.f, 1.f});
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