#include "arpch.h"
#include "ComponentInspector.h"
#include "PropertyInspector.h"


void ar::ComponentInspector::ShowInspector(Entity entity)
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	std::string line = entity.GetName() + " [ID: " + std::to_string(entity.GetID()) + "]";
	ImGui::SeparatorText(line.c_str());

	if (ImGui::BeginTabBar("Inspector", tab_bar_flags))
	{
		ShowComponentInspector<ar::TorusComponent>(entity, "Props");
		ShowComponentInspector<ar::MeshComponent>(entity, "Mesh");
		ShowComponentInspector<ar::TransformComponent>(entity, "Transform");
		
		// rest...
		ImGui::EndTabBar();
	}
}

void ar::ComponentInspector::InspectComponent(TorusComponent& torus)
{
	ImGui::Text("Hey this is torus properties");
}

void ar::ComponentInspector::InspectComponent(TransformComponent& transform)
{
	ImGui::Text("Hey this is transform");
}

void ar::ComponentInspector::InspectComponent(MeshComponent& mesh)
{
	ImGui::Text("Hey this is mesh");
}
