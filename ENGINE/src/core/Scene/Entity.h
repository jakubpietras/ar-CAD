#pragma once
#include <entt/entt.hpp>
#include <memory>
#include "Components.h"
#include "Scene.h"
#include "LogUtils.h"

namespace ar
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			if constexpr (std::is_empty_v<T>)
			{
				m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
				// Return a dummy reference to a static instance (safe because it's stateless)
				static T dummy;
				return dummy;
			}
			else
			{
				T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
				return component;
			}
		}

		template<typename T>
		T& GetComponent()
		{
			AR_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent() const
		{
			AR_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			AR_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		template<typename... Components>
		bool HasAnyComponent()
		{
			return (m_Scene->m_Registry.any_of<Components>(m_EntityHandle) || ...);
		}

		inline bool IsValid() const { return m_Scene->m_Registry.valid(m_EntityHandle); }
		inline const entt::entity GetEnttHandle() const { return m_EntityHandle; }


		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		const uint32_t GetID();
		const std::string& GetName();
		void SetName(const std::string& newName);
		void Hide();
		void Show();

		VertexPositionID GetPositionID();

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		struct HashFunction
		{
			size_t operator()(const ar::Entity& e) const noexcept
			{
				return std::hash<uint32_t>{}(static_cast<uint32_t>(e));
			}
		};

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};



}

