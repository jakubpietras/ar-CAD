#include "arpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "core/UID.h"
#include "core/Renderer/Framebuffer.h"
#include <core/Utils/CurveUtils.h>
#include "core/Utils/GeneralUtils.h"
#include "core/Geometry/GregoryFill.h"

namespace ar
{

	Scene::Scene()
	{
		AR_INFO("Scene initialized!");
	}

	Scene::~Scene() { }

	Entity Scene::CreateEntity(std::optional<uint32_t> id, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		auto& ic = entity.AddComponent<IDComponent>();
		if (id.has_value())
		{
			ic.ID = *id;
			UID::AddUsed(*id);
		}
		else
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

	void Scene::OnUpdate(Ref<PerspectiveCamera> camera, ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos)
	{
		UpdateScene(cursorPos, meanPos);
	}

	void Scene::UpdateScene(ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos)
	{
		FlagDirtyMeshes();
		UpdateAllTransforms(cursorPos, meanPos);
		UpdateMeshes();
		UpdateGregory();
		UpdateTori();
	}

	void Scene::UpdateTransform(TransformComponent& tc, ar::mat::Vec3 pivot)
	{
		if (!tc.DirtyFlag)
			return;

		if (tc.GroupTransformation)
		{
			// ---- Scale ----
			auto v = tc.PreviousTranslation - pivot;
			auto d_scale = tc.Scale / tc.PreviousScale;
			auto scaled_v = d_scale * v;
			auto scaledPos = pivot + scaled_v;

			// ---- Rotation ----
			v = scaledPos - pivot;
			auto totalRotationDelta = tc.AnglesRPY - tc.PreviousAnglesRPY;
			auto q = mat::RPYToQuat(totalRotationDelta);
			auto rotated_v = q * mat::Quat(0.0f, v.x, v.y, v.z) * mat::Conjugate(q);
			auto rotatedPos = pivot + QuatToVec3(rotated_v);

			// ---- Translation ----
			tc.Translation = rotatedPos + (tc.Translation - tc.PreviousTranslation);
			tc.Rotation = mat::RPYToQuat(tc.AnglesRPY);
		}
		else
		{
			// ---- Scale ----
			auto v = tc.Translation - pivot;
			auto d_scale = tc.Scale / tc.PreviousScale;
			auto scaled_v = d_scale * v;
			tc.Translation = pivot + scaled_v;

			// ---- Rotation ----
			v = tc.Translation - pivot;
			auto q = mat::RPYToQuat(tc.AnglesRPY - tc.PreviousAnglesRPY);
			auto rotated_v = q * mat::Quat(0.0f, v.x, v.y, v.z) * mat::Conjugate(q);
			tc.Translation = pivot + QuatToVec3(rotated_v);
			tc.Rotation = mat::Normalize(q * tc.Rotation);
		}

		// ---- Combination ----
		tc.ModelMatrix =
			mat::TranslationMatrix(tc.Translation) *
			mat::ToMat4(tc.Rotation) *
			mat::ScaleMatrix(tc.Scale);
		tc.DirtyFlag = false;
	}

	void Scene::FlagDirtyMeshes()
	{
		auto pointView = m_Registry.view<PointComponent, TransformComponent>();
		for (auto [entity, pc, tc] : pointView.each())
		{
			if (!tc.DirtyFlag)
				continue;
			for (auto parent : pc.Parents)
			{
				parent.GetComponent<MeshComponent>().DirtyFlag = true;
			}
		}
	}

	void Scene::UpdateAllTransforms(ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos)
	{
		auto transformView = m_Registry.view<TransformComponent>();
		for (auto [entity, tc] : transformView.each())
		{
			if (tc.GroupTransformation)
				UpdateTransform(tc, cursorPos);
			else if (tc.PivotPoint == PivotType::CURSOR)
				UpdateTransform(tc, cursorPos);
			else
				UpdateTransform(tc, tc.Translation);
		}
	}

	void Scene::UpdateMeshes()
	{
		auto meshView = m_Registry.view<ControlPointsComponent, MeshComponent>(entt::exclude<ar::GregoryPatchComponent>);
		for (auto [entity, cp, mesh] : meshView.each())
		{
			if (!mesh.DirtyFlag)
				continue;

			auto e = ar::Entity(entity, this);
			mesh.VertexArray->ClearBuffers();

			std::vector<VertexPositionID> verts;
			if (e.HasComponent<ar::InterpolatedC2Component>())
				verts = ar::CurveUtils::GetIntC2VertexData(cp.Points, e.GetID());
			else
				verts = ar::GeneralUtils::GetVertexData(cp.Points, e.GetID());

			auto vb = ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(verts));
			mesh.VertexArray->AddVertexBuffer(vb);
			if (e.HasComponent<ControlMeshComponent>())
			{
				auto& cm = e.GetComponent<ControlMeshComponent>();
				cm.VertexArray->ClearVertexBuffers();
				cm.VertexArray->AddVertexBuffer(vb);
				if (e.HasComponent<SurfaceComponent>())
				{
					auto& surf = e.GetComponent<ar::SurfaceComponent>();
					if (surf.Description.Type == SurfaceType::RECTANGLEC2 
						|| surf.Description.Type == SurfaceType::CYLINDERC2)
						surf.AuxPoints = ar::SurfaceUtils::GetBezierFromDeBoor(e);
				}
			}

			if (e.HasComponent<ar::CurveC0Component>())
			{
				// todo (maybe): limit generation of new indices to only when vertex count changes
				auto indices = ar::CurveUtils::GenerateC0Indices(cp.Points.size());
				mesh.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(indices)));
			}
			if (e.HasComponent<ar::CurveC2Component>())
			{
				// todo (maybe): limit generation of new indices to only when vertex count changes
				auto indices = ar::CurveUtils::GenerateC2Indices(cp.Points.size());
				mesh.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(indices)));
			}

			if (!cp.Indices.empty())
			{
				mesh.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(cp.Indices)));
			}

			mesh.DirtyFlag = false;
		}
	}

	void Scene::UpdateGregory()
	{
		auto view = m_Registry.view<GregoryPatchComponent, MeshComponent, ControlMeshComponent>();
		
		for (auto [entity, gp, mc, cmc] : view.each())
		{
			auto e = ar::Entity(entity, this);
			
			if (!mc.DirtyFlag)
				continue;

			auto newVB = ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GregoryFill::GetGregoryVertexData(gp.HoleToFill, e.GetID())));
			mc.VertexArray->ClearBuffers();
			mc.VertexArray->AddVertexBuffer(newVB);

			cmc.VertexArray->ClearVertexBuffers();
			cmc.VertexArray->AddVertexBuffer(newVB);
		}

	}

	void Scene::UpdateTori()
	{
		auto torusView = m_Registry.view<TorusComponent, MeshComponent>();
		for (auto [entity, toc, mc] : torusView.each())
		{
			if (!toc.DirtyFlag)
				continue;
			auto e = ar::Entity(entity, this);
			mc.VertexArray->ClearBuffers();
			toc.Vertices = ar::TorusUtils::GenerateTorusVertices(toc.Description, e.GetID());
			toc.Edges = ar::TorusUtils::GenerateTorusEdges(toc.Description);
			mc.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(toc.Vertices)));
			auto indexBuffers = ar::IndexBuffer::Create(toc.Edges);
			for (auto& ib : indexBuffers)
				mc.VertexArray->AddIndexBuffer(ib);
			toc.DirtyFlag = false;
		}
	}

}