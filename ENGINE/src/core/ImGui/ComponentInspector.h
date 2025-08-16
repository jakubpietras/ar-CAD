#pragma once
#include "core/Scene/Entity.h"
#include "core/Scene/Components.h"
#include <imgui.h>

namespace ar
{
	class ComponentInspector
	{
	public:
		static void ShowInspector(Entity entity);

	private:
		template<typename T>
		static void ShowComponentInspector(Entity entity, const char* name)
		{
			if (!entity.HasComponent<T>()) return;

			if (ImGui::BeginTabItem(name))
			{
				auto& component = entity.GetComponent<T>();
				InspectComponent(component);
				ImGui::EndTabItem();
			}
		}

		static void InspectComponent(TorusComponent& torus);
		static void InspectComponent(MeshComponent& mesh);
		static void InspectComponent(TransformComponent& transform);
		static void InspectComponent(CurveC0Component& curve);
	};
}
