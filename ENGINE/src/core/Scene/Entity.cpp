#include "arpch.h"
#include "Entity.h"
#include "Components.h"

namespace ar
{

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{ }

	void Entity::SetName(const std::string& newName)
	{
		auto& name = GetComponent<TagComponent>();
		name.Tag = newName;
	}

	uint32_t Entity::GetID()
	{
		return GetComponent<IDComponent>().ID;
	}

	const std::string& Entity::GetName()
	{
		return GetComponent<TagComponent>().Tag;
	}

}