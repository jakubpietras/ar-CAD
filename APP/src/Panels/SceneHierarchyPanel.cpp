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
	bool shiftPressed = ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT);

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
		if (!shiftPressed)
		{
			if (!isSelected || m_State.SelectedObjects.size() > 1)
			{
				RequestObjectSelect(object, false);
			}
		}
		else
		{
			if (isSelected)
				RequestObjectDeselect(object);
			else
				RequestObjectSelect(object, true);
		}
	}

	if (ImGui::BeginPopupContextItem())
	{
		DrawEntityContextMenu(object, !selectionEmpty, true);
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
	bool shiftPressed = ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT);

	if (isSelected)
		flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::TreeNodeEx(
		(void*)(intptr_t)child.GetID(),
		flags,
		"%s", child.GetName().c_str()
	);

	if (ImGui::IsItemClicked())
	{
		if (!shiftPressed)
		{
			if (!isSelected || m_State.SelectedObjects.size() > 1)
			{
				RequestObjectSelect(child, false);
			}
		}
		else
		{
			if (isSelected)
				RequestObjectDeselect(child);
			else
				RequestObjectSelect(child, true);
		}
	}

	if (ImGui::BeginPopupContextItem())
	{
		DrawEntityContextMenu(child, false, false); // no Delete Selected here
		ImGui::Separator();
		DrawLinkContextMenu(child, parent);
		ImGui::EndPopup();
	}
}

void SceneHierarchyPanel::DrawEntityContextMenu(ar::Entity& object, bool allowDeleteSelected, bool allowAttachment)
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
	if (allowAttachment)
	{
		ar::ScopedDisable disable(m_State.SelectedCurves.empty() || !object.HasComponent<ar::PointComponent>());
		if (ImGui::MenuItem("Attach to selected curves"))
		{
			for (auto& curve : m_State.SelectedCurves)
				m_State.PairsToAttach.push_back({ curve, object });
			m_State.ShowAttachModal = true;
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

void SceneHierarchyPanel::RequestObjectSelect(ar::Entity object, bool add)
{
	m_State.SelectionCandidates.clear();
	m_State.SelectionCandidates.push_back(object);
	m_State.SelectionChangeMode = add ? SelectionMode::Add : SelectionMode::Replace;
	m_State.ShouldUpdateSelection = true;
}

void SceneHierarchyPanel::RequestObjectDeselect(ar::Entity object)
{
	m_State.SelectionCandidates.clear();
	m_State.SelectionCandidates.push_back(object);
	m_State.SelectionChangeMode = SelectionMode::Remove;
	m_State.ShouldUpdateSelection = true;
}

