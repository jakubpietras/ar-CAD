#include "arpch.h"
#include "Scene.h"
#include "Entity.h"
#include "core/Renderer/Renderer.h"
#include "core/Commands/CommandQueue.h"
#include "Components.h"
#include "core/UID.h"

namespace ar
{

	Scene::Scene() 
	{
		AR_INFO("Scene initialized!");
		m_CubeShader = std::shared_ptr<ar::Shader>(
			ar::Shader::Create("resources/shaders/OpenGL/default.vert",
				"resources/shaders/OpenGL/default.frag"));
		m_GridShader = std::shared_ptr<ar::Shader>(
			ar::Shader::Create("resources/shaders/OpenGL/grid.vert",
				"resources/shaders/OpenGL/grid.frag"));
		m_BasicShader = std::shared_ptr<ar::Shader>(
			ar::Shader::Create("resources/shaders/OpenGL/basic.vert",
				"resources/shaders/OpenGL/basic.frag"));
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

	void Scene::RenderScene(std::shared_ptr<PerspectiveCamera> camera)
	{
		ar::Renderer::BeginScene();

		auto& vpMat = camera->GetVP();

		// Draw meshes (e.g. tori)
		{
			auto view = m_Registry.view<MeshComponent>(entt::exclude<PointTagComponent>);
			for (auto [entity, mc] : view.each())
			{
				mc.Shader->SetMat4("u_VP", vpMat);
				ar::Renderer::Submit(mc);
			}
		}

		// Draw points
		{
			// todo
		}


		// Draw grid
		{
			m_GridShader->SetMat4("u_VP", vpMat);
			ar::Renderer::Submit(ar::Primitive::Triangle, m_GridShader, 6);
		}

		ar::Renderer::EndScene();
	}

}