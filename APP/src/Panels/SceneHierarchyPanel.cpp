#include "SceneHierarchyPanel.h"

SceneHierarchyPanel::SceneHierarchyPanel(std::shared_ptr<ar::Scene> scene, std::shared_ptr<ar::CommandQueue> commandQueue)
	: m_Context(scene), m_CommandQueue(commandQueue)
{ }

SceneHierarchyPanel::~SceneHierarchyPanel()
{ }

void SceneHierarchyPanel::SetContext(std::shared_ptr<ar::Scene> scene)
{
	m_Context = scene;
}

void SceneHierarchyPanel::OnImGuiRender()
{
	ImGui::Begin("Scene Hierarchy");
	if (m_Context)
	{
		for (auto entity : m_Context->m_Registry.view<entt::entity>(entt::exclude<ar::VirtualTagComponent>))
		{
			ar::Entity e{ entity, m_Context.get() };
			DrawTreeNode(e);
		}
	}
	ImGui::End();


	ImGui::Begin("Inspector");

	if (!m_Context->m_Registry.view<ar::SelectedTagComponent>().empty())
	{
		auto e = m_Context->GetLastSelectedEntity();
		auto object = ar::Entity{ e, m_Context.get() };
		ImGui::Text("Test of the inspector");
		auto line = object.GetName() + std::to_string(object.GetID());
		ImGui::Text(line.c_str());

	}
	
	ImGui::End();
}

void SceneHierarchyPanel::DrawTreeNode(ar::Entity& object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull;

	if (object.HasComponent<ar::TorusComponent>())
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (object.HasComponent<ar::SelectedTagComponent>())
			flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)object.GetID(), flags, "%s", object.GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			if (m_Context->IsEntitySelected(object))
				m_CommandQueue->Execute(std::move(std::make_unique<ar::DeselectObjectCommand>(object)));
			else
				m_CommandQueue->Execute(std::move(std::make_unique<ar::SelectObjectCommand>(object)));
		}
	}


}
