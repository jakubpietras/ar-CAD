#include "arpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "core/UID.h"
#include "core/Renderer/Framebuffer.h"

namespace ar
{

	Scene::Scene()
		: m_PickingFB(std::shared_ptr<Framebuffer>(Framebuffer::Create({ 1920, 1080, 1, TextureFormat::R32 })))
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

	void Scene::ResizePickingFramebuffer(float width, float height)
	{
		m_PickingFB->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}

	void Scene::BeginPicking()
	{
		m_PickingFB->Bind();

		GLuint clearValue = 0;
		glClearBufferuiv(GL_COLOR, 0, &clearValue);

		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void Scene::EndPicking()
	{
		m_PickingFB->Unbind();
	}

	uint32_t Scene::ReadPixel(int x, int y)
	{
		uint32_t pixelID = 0;
		m_PickingFB->Bind();

		// Debug: Read entire framebuffer to see if there's ANY non-zero data
		int width = 100, height = 100; // or your actual FB dimensions
		std::vector<uint32_t> allPixels(width * height);
		glReadPixels(0, 0, width, height, GL_RED, GL_UNSIGNED_INT, allPixels.data());

		uint32_t nonZeroCount = 0;
		uint32_t maxValue = 0;
		for (uint32_t val : allPixels) {
			if (val != 0) {
				nonZeroCount++;
				maxValue = std::max(maxValue, val);
			}
		}

		AR_TRACE("Non-zero pixels: {0}, Max value: {1}", nonZeroCount, maxValue);

		// Original read
		glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_INT, &pixelID);
		m_PickingFB->Unbind();

		return pixelID;
	}

	void Scene::OnUpdate(Ref<PerspectiveCamera> camera, ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos)
	{
		UpdateScene(cursorPos, meanPos);
	}

	void Scene::UpdateScene(ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos)
	{
		auto transformView = m_Registry.view<TransformComponent>();
		for (auto [entity, tc] : transformView.each())
		{
			UpdateTransform(tc, cursorPos, meanPos);
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

	void Scene::UpdateTransform(TransformComponent& tc, ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos)
	{
		if (!tc.DirtyFlag)
			return;

		ar::mat::Vec3 pivot;
		switch (tc.PivotPoint)
		{
			case PivotType::CURSOR:
			{
				pivot = cursorPos;
				break;
			}
			case PivotType::MEAN_SELECTED:
			{
				pivot = meanPos;
				break;
			}
			case PivotType::LOCAL_ORIGIN:
			default:
			{
				pivot = tc.Translation;
			}
		}

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

		// ---- Combination ----
		tc.ModelMatrix =
			mat::TranslationMatrix(tc.Translation) *
			mat::ToMat4(tc.Rotation) *
			mat::ScaleMatrix(tc.Scale);

		tc.DirtyFlag = false;
	}

}