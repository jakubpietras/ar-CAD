#pragma once
#include "core/Scene/Entity.h"
#include "core/Scene/Components.h"
#include <imgui.h>
#include "core/Commands/Command.h"
using CommandSubmitFn = std::function<void(std::unique_ptr<ar::Command>)>;

namespace ar
{
	class ComponentInspector
	{
	public:
		static void ShowInspector(Entity entity, CommandSubmitFn submit);

	private:
		template<typename T>
		static void ShowComponentInspector(Entity entity, const char* name, CommandSubmitFn submit)
		{
			if (!entity.HasComponent<T>()) return;

			if (ImGui::BeginTabItem(name))
			{
				auto& component = entity.GetComponent<T>();
				InspectComponent(component, submit);
				ImGui::EndTabItem();
			}
		}

		static void InspectComponent(TorusComponent& torus, CommandSubmitFn submit);
		static void InspectComponent(MeshComponent& mesh, CommandSubmitFn submit);
		static void InspectComponent(TransformComponent& transform, CommandSubmitFn submit);
	};
}
