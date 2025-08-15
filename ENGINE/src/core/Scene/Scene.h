#pragma once
#include "entt/entt.hpp"
#include "core/Renderer/Camera.h"
#include "core/Renderer/Shader.h"
#include "core/Scene/Components.h"
#include "core/Renderer/Renderer.h"
#include "core/Renderer/RenderCommand.h"
#include "core/Renderer/Framebuffer.h"

namespace ar
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		
		void OnUpdate(Ref<PerspectiveCamera> camera, ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos);

		// Entity management
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		Entity FindEntityByName(const std::string& name);
		Entity GetEntityByID(uint32_t id);
		inline uint32_t GetEntityCount() { return static_cast<uint32_t>(m_EntityMap.size()); }
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
		entt::registry m_Registry;
	private:
		std::unordered_map<uint32_t, entt::entity> m_EntityMap;

		// Updates
		void UpdateScene(ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos);
		void UpdateTransform(TransformComponent& tc, ar::mat::Vec3 pivot);
		void FlagDirtyMeshes();
		void UpdateAllTransforms(ar::mat::Vec3 cursorPos, ar::mat::Vec3 meanPos);
		void UpdateMeshes();
		void UpdateTori();

		

		friend class Entity;
		friend class SceneHierarchyPanel;
	};

}

