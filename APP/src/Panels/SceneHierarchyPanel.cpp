#include "SceneHierarchyPanel.h"
#include "core/ImGui/ScopedDisable.h"

SceneHierarchyPanel::SceneHierarchyPanel(EditorState& state)
	: m_State(state) { }

void SceneHierarchyPanel::Render()
{
	AR_ASSERT(m_Scene, "Context (Scene) not set!");

	ImGui::Begin("Scene Hierarchy");
	
	for (auto entity : m_Scene->m_Registry.view<entt::entity>(entt::exclude<ar::VirtualTagComponent>))
	{
		ar::Entity e{ entity, m_Scene.get() };
		DrawParentNode(e);
	}

	ImGui::End();
}

void SceneHierarchyPanel::SetContext(ar::Ref<ar::Scene> scene)
{
	m_Scene = scene;
}

void SceneHierarchyPanel::DrawParentNode(ar::Entity object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull;
	bool hasChildren = object.HasComponent<ar::ControlPointsComponent>(),
		isSelected = object.HasComponent<ar::SelectedTagComponent>(),
		selectionEmpty = m_State.SelectedObjects.empty();

	if (!hasChildren)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (isSelected)
		flags |= ImGuiTreeNodeFlags_Selected;

	bool nodeOpen = ImGui::TreeNodeEx(
		(void*)(intptr_t)object.GetID(),
		flags,
		"%s", object.GetName().c_str()
	);

	if (ImGui::IsItemClicked())
	{
		if (!ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
			m_State.ClearSelectionState();
		if (isSelected)
			DeselectObject(object);
		else
			SelectObject(object);
	}

	if (ImGui::BeginPopupContextItem())
	{
		DrawEntityContextMenu(object, !selectionEmpty);
		ImGui::EndPopup();
	}

	if (nodeOpen && hasChildren)
	{
		for (auto& point : object.GetComponent<ar::ControlPointsComponent>().Points)
			DrawChildNode(point, object);

		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawChildNode(ar::Entity child, ar::Entity parent)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull |
		ImGuiTreeNodeFlags_Leaf |
		ImGuiTreeNodeFlags_NoTreePushOnOpen;
	bool isSelected = child.HasComponent<ar::SelectedTagComponent>(); 

	if (isSelected)
		flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::TreeNodeEx(
		(void*)(intptr_t)child.GetID(),
		flags,
		"%s", child.GetName().c_str()
	);

	if (ImGui::IsItemClicked())
	{
		if (!ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
			m_State.ClearSelectionState();
		if (isSelected)
			DeselectObject(child);
		else
			SelectObject(child);
	}

	if (ImGui::BeginPopupContextItem())
	{
		DrawEntityContextMenu(child, false); // no Delete Selected here
		ImGui::Separator();
		DrawLinkContextMenu(child, parent);
		ImGui::EndPopup();
	}
}

void SceneHierarchyPanel::DrawEntityContextMenu(ar::Entity& object, bool allowDeleteSelected)
{
	if (ImGui::MenuItem("Rename"))
	{
		m_State.ObjectToRename = object;
		m_State.ShowRenameModal = true;
	}
	if (ImGui::MenuItem("Delete"))
	{
		m_State.ObjectsToDelete.push_back(object);
		m_State.ShowDeleteModal = true;
	}
	if (allowDeleteSelected)
	{
		// "Delete selected" is disabled when there are no objects selected, or context menu is
		// spawned on an item that is not currently selected
		ar::ScopedDisable disabled(
			m_State.SelectedObjects.empty() ||
			!object.HasComponent<ar::SelectedTagComponent>()
		);
		if (ImGui::MenuItem("Delete Selected"))
		{
			m_State.ObjectsToDelete.insert(
				m_State.ObjectsToDelete.end(),
				m_State.SelectedObjects.begin(),
				m_State.SelectedObjects.end()
			);
			m_State.ShowDeleteModal = true;
		}
	}
}

void SceneHierarchyPanel::DrawLinkContextMenu(ar::Entity& child, ar::Entity& parent)
{
	if (ImGui::MenuItem("Detach"))
	{
		m_State.PairsToDetach.push_back({ parent, child });
		m_State.ShowDetachModal = true;
	}
}

void SceneHierarchyPanel::SelectObject(ar::Entity object)
{
	object.AddComponent<ar::SelectedTagComponent>();
	m_State.SelectedObjects.push_back(object);

	if (object.HasComponent<ar::PointTagComponent>())
		m_State.SelectedPoints.push_back(object);
}

void SceneHierarchyPanel::DeselectObject(ar::Entity object)
{
	auto& objs = m_State.SelectedObjects;
	objs.erase(std::remove(objs.begin(), objs.end(), object), objs.end());

	if (object.HasComponent<ar::PointTagComponent>())
	{
		auto& pts = m_State.SelectedPoints;
		pts.erase(std::remove(pts.begin(), pts.end(), object), pts.end());
	}
	object.RemoveComponent<ar::SelectedTagComponent>();
}

