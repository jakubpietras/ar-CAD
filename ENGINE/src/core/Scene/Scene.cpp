#include "arpch.h"
#include "Scene.h"
#include "Entity.h"
#include "core/Renderer/Renderer.h"
#include "core/Renderer/RenderCommand.h"
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

		// Draw grid
		{
			auto gridShader = ar::ShaderLib::Get("Grid");
			ar::RenderCommand::ToggleDepthTest(false);
			gridShader->SetMat4("u_VP", vpMat);
			ar::Renderer::Submit(ar::Primitive::Triangle, gridShader, 6);
			ar::RenderCommand::ToggleDepthTest(true);
		}

		// Draw meshes (e.g. tori)
		{
			auto view = m_Registry.view<MeshComponent, TransformComponent>(entt::exclude<PointTagComponent>);
			for (auto [entity, mc, tc] : view.each())
			{
				mc.Shader->SetMat4("u_VP", vpMat);
				mc.Shader->SetMat4("u_Model", tc.ModelMatrix);
				ar::Renderer::Submit(mc);
			}
		}

		auto shader = ShaderLib::Get("Basic");
		shader->SetMat4("u_VP", camera->GetVP());
		shader->SetMat4("u_Model", mat::Identity());

		// Draw lines
		{
			auto view = m_Registry.view<ControlPointsComponent>();
			for (auto [entity, cp] : view.each())
			{
				std::vector<VertexPosition> verts{};
				for (auto& point : cp.Points)
				{
					verts.push_back({ point.GetComponent<TransformComponent>().Translation });
				}

				auto ent = ar::Entity{ entity, this };
				if (ent.HasComponent<ChainTagComponent>())
				{
					m_PointsVA->ClearBuffers();
					m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(verts)));
					ar::Renderer::Submit(Primitive::LineStrip, shader, m_PointsVA);
				}
			}
		}

		// Draw points
		{
			std::vector<VertexPosition> pointPositions{};
			auto view = m_Registry.view<TransformComponent, PointTagComponent>();
			for (const auto& [e, transform] : view.each())
				pointPositions.push_back({ transform.Translation });
			m_PointsVA->ClearBuffers();
			m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(pointPositions)));

			ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
		}

		ar::Renderer::EndScene();
	}


}