#include "arpch.h"
#include "Entity.h"

namespace ar
{

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{ }
}