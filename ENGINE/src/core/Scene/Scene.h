#pragma once
#include "entt/entt.hpp"
#include "core/Renderer/Camera.h"
#include "core/Renderer/Shader.h"

namespace ar
{
	class Entity;
	class CommandQueue;

	class Scene
	{
	public:
		Scene();
		~Scene();
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		
		// Entity management
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		Entity FindEntityByName(const std::string& name);
		Entity GetEntityByID(uint32_t id);
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
		inline uint32_t GetEntityCount() { return m_EntityMap.size(); }

		void RenderScene(std::shared_ptr<PerspectiveCamera> camera);

		// Cursor
		// todo: implement cursor logic
		inline mat::Vec3 GetCursorPos() { return { 0.0f, 0.0f, 0.0f }; }
		
		// Accessors
		inline std::shared_ptr<ar::Shader>& GetBasicShader() { return m_BasicShader; }

		entt::registry m_Registry;
	private:
		std::unordered_map<uint32_t, entt::entity> m_EntityMap;

		std::shared_ptr<ar::Shader> m_CubeShader, m_GridShader, m_BasicShader;

		// ECS Systems
		
		friend class Entity;
		friend class SceneHierarchyPanel;
	};

}

