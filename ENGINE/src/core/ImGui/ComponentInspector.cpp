#include "arpch.h"
#include "core/Utils/TorusUtils.h"
#include "ComponentInspector.h"
#include "PropertyInspector.h"
#include "core/ImGui/ScopedDisable.h"


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
	if (PropertyInspector::InspectProperty("Small Radius", torus.Description.SmallRadius, 0.1f, 10.f))
		torus.DirtyFlag = true;
	if (PropertyInspector::InspectProperty("Large Radius", torus.Description.LargeRadius, 0.1f, 10.f))
		torus.DirtyFlag = true;
	if (PropertyInspector::InspectProperty("Samples", torus.Description.Samples, 3u, 64u))
		torus.DirtyFlag = true;

}

void ar::ComponentInspector::InspectComponent(TransformComponent& transform)
{
	if (PropertyInspector::InspectProperty("Translation", transform.Translation, transform.PreviousTranslation, -20.0f, 20.0f))
		transform.DirtyFlag = true;
	{
		ScopedDisable disabled(!transform.IsRotationEnabled);
		if (PropertyInspector::InspectProperty("Rotation", transform.AnglesRPY, transform.PreviousAnglesRPY, -180.0f, 180.0f))
			transform.DirtyFlag = true;
	}
	{
		ScopedDisable disabled(!transform.IsScaleEnabled);
		if (PropertyInspector::InspectProperty("Scale", transform.Scale, transform.PreviousScale, 0.1f, 10.0f))
			transform.DirtyFlag = true;
	}
}

void ar::ComponentInspector::InspectComponent(MeshComponent& mesh)
{
	ImGui::Text("Hey this is mesh");
}
