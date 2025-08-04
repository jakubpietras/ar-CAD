#include "arpch.h"
#include "ComponentInspector.h"
#include "PropertyInspector.h"
#include "core/Commands/SceneCommands.h"


void ar::ComponentInspector::ShowInspector(Entity entity, CommandSubmitFn submit)
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	std::string line = entity.GetName() + " [ID: " + std::to_string(entity.GetID()) + "]";
	ImGui::SeparatorText(line.c_str());

	if (ImGui::BeginTabBar("Inspector", tab_bar_flags))
	{
		ShowComponentInspector<ar::TorusComponent>(entity, "Props", submit);
		ShowComponentInspector<ar::MeshComponent>(entity, "Mesh", submit);
		ShowComponentInspector<ar::TransformComponent>(entity, "Transform", submit);
		
		// rest...
		ImGui::EndTabBar();
	}
}

void ar::ComponentInspector::InspectComponent(TorusComponent& torus, CommandSubmitFn submit)
{
	ImGui::Text("Hey this is torus properties");
}

void ar::ComponentInspector::InspectComponent(TransformComponent& transform, CommandSubmitFn submit)
{
	auto translation = PropertyInspector::InspectProperty("Translation", transform.Translation, 
		transform.PreviousTranslation, -10.f, 10.f);
	if (translation.Activated)
		transform.PreviousTranslation = transform.Translation;
	if (translation.Changed)
		transform.DirtyFlag = true;
	if (translation.Committed)
		submit(std::make_unique<SetPropertyCommand<mat::Vec3>>(
			&transform.Translation, transform.PreviousTranslation, transform.Translation,
			&transform.DirtyFlag));

	auto rotation = PropertyInspector::InspectProperty("Rotation", transform.AnglesRPY,
		transform.PreviousAnglesRPY, -10.f, 10.f);
	if (rotation.Activated)
		transform.PreviousAnglesRPY = transform.AnglesRPY;
	if (rotation.Changed)
		transform.DirtyFlag = true;
	if (rotation.Committed)
		submit(std::make_unique<SetPropertyCommand<mat::Vec3>>(
			&transform.AnglesRPY, transform.PreviousAnglesRPY, transform.AnglesRPY,
			&transform.DirtyFlag));
}

void ar::ComponentInspector::InspectComponent(MeshComponent& mesh, CommandSubmitFn submit)
{
	ImGui::Text("Hey this is mesh");
}
