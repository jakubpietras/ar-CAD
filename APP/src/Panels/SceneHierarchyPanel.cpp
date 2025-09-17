#include "SceneHierarchyPanel.h"
#include "core/ImGui/ScopedDisable.h"
#include "core/Scene/Components.h"

SceneHierarchyPanel::SceneHierarchyPanel(EditorState& state)
	: m_State(state) { }

void SceneHierarchyPanel::Render()
{
	AR_ASSERT(m_Scene, "Context (Scene) not set!");
	auto view = m_Scene->m_Registry.view<entt::entity>(entt::exclude<ar::VirtualTagComponent>);

	ImGui::Begin("Scene");
	ImGui::SeparatorText("Mesh visibility");

	if (ImGui::Button("Show all"))
	{
		for (auto entity : view)
		{
			ar::Entity e{ entity, m_Scene.get() };
			m_State.ObjectsToShow.insert(e);
		}
		m_State.ShouldUpdateVisibility = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Hide all"))
	{
		for (auto entity : view)
		{
			ar::Entity e{ entity, m_Scene.get() };
			m_State.ObjectsToHide.insert(e);
		}
		m_State.ShouldUpdateVisibility = true;
	}

	ImGui::TextWrapped("Objects that are hidden in the scene will appear teal in the hierarchy.");

	ImGui::SeparatorText("Hierarchy");
	ImGui::Checkbox("Display unselected points", &m_ShowUnselectedPoints);
	for (auto entity : view)
	{
		ar::Entity e{ entity, m_Scene.get() };
		if (e.HasComponent<ar::PointComponent>() && !e.HasComponent<ar::SelectedTagComponent>() && !m_ShowUnselectedPoints)
			continue;
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
	bool isHidden = object.HasComponent<ar::HiddenTagComponent>();

	if (!hasChildren)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (isSelected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (isHidden)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.7f, 1.0f));
	bool nodeOpen = ImGui::TreeNodeEx(
		(void*)(intptr_t)object.GetID(),
		flags,
		"%s", object.GetName().c_str()
	);
	if (isHidden)
		ImGui::PopStyleColor();

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

	int counter = 0;
	if (nodeOpen && hasChildren)
	{
		for (auto& point : object.GetComponent<ar::ControlPointsComponent>().Points)
			DrawChildNode(point, object, counter++);

		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawChildNode(ar::Entity child, ar::Entity parent, int counter)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull |
		ImGuiTreeNodeFlags_Leaf |
		ImGuiTreeNodeFlags_NoTreePushOnOpen;
	bool isSelected = child.HasComponent<ar::SelectedTagComponent>(); 
	bool shiftPressed = ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT);
	bool isHidden = child.HasComponent<ar::HiddenTagComponent>();

	if (isSelected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (isHidden)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.7f, 1.0f));
	auto label = std::to_string(child.GetID()) + "##" + std::to_string(counter);
	ImGui::TreeNodeEx(
		label.c_str(),
		flags,
		"%s", child.GetName().c_str()
	);
	if (isHidden)
		ImGui::PopStyleColor();

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
	ImGui::SeparatorText("Object controls");
	if (ImGui::MenuItem("Rename"))
	{
		m_State.ObjectToRename = object;
		m_State.ShowRenameModal = true;
	}
	{
		ar::ScopedDisable disabled(object.HasComponent<ar::SurfacePointTagComponent>());
		if (ImGui::MenuItem("Delete"))
		{
			m_State.ObjectsToDelete.insert(object);
			if (object.HasComponent<ar::SurfaceComponent>())
			{
				// delete all points contained in the surface
				auto& points = object.GetComponent<ar::ControlPointsComponent>().Points;
				m_State.ObjectsToDelete.insert(points.begin(), points.end());
			}
			m_State.ShowDeleteModal = true;
		}
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
				m_State.SelectedObjects.begin(),
				m_State.SelectedObjects.end()
			);
			m_State.ShowDeleteModal = true;
		}
	}
	ImGui::SeparatorText("Curve/surface controls");
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
	if (object.HasComponent<ar::SurfaceComponent>())
		if (ImGui::MenuItem("Select all points"))
		{
			m_State.SelectionChangeMode = SelectionMode::Replace;
			m_State.SelectionCandidates.clear();
			auto& children = object.GetComponent<ar::ControlPointsComponent>().Points;
			m_State.SelectionCandidates.insert(
				m_State.SelectionCandidates.begin(),
				children.begin(),
				children.end()
			);
			m_State.ShouldUpdateSelection = true;
		}
	ImGui::SeparatorText("Point controls");
	if (object.HasComponent<ar::PointComponent>())
	{
		if (ImGui::MenuItem("Collapse"))
		{
			m_State.ShowCollapseModal = true;
		}
	}
	ImGui::SeparatorText("Visibility controls");
	if (ImGui::MenuItem("Show exclusive"))
	{
		auto view = m_Scene->m_Registry.view<entt::entity>(entt::exclude<ar::VirtualTagComponent>);
		for (auto entity : view)
		{
			ar::Entity e(entity, m_Scene.get());
			RequestHideObject(e);
		}
		RequestShowObject(object);
	}
}

void SceneHierarchyPanel::DrawLinkContextMenu(ar::Entity& child, ar::Entity& parent)
{
	ImGui::SeparatorText("Link controls");
	bool surfaceParent = parent.HasComponent<ar::SurfaceComponent>();
	{
		ar::ScopedDisable disable(surfaceParent);
		if (ImGui::MenuItem("Detach"))
		{
			m_State.PairsToDetach.push_back({ parent, child });
			m_State.ShowDetachModal = true;
		}
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

void SceneHierarchyPanel::RequestHideObject(ar::Entity object)
{
	m_State.ObjectsToHide.insert(object);
	if (object.HasComponent<ar::ControlPointsComponent>())
	{
		auto& points = object.GetComponent<ar::ControlPointsComponent>().Points;
		for (auto& point : points)
			m_State.ObjectsToHide.insert(point);
	}
	m_State.ShouldUpdateVisibility = true;
}

void SceneHierarchyPanel::RequestShowObject(ar::Entity object)
{
	m_State.ObjectsToShow.insert(object);
	if (object.HasComponent<ar::ControlPointsComponent>())
	{
		auto& points = object.GetComponent<ar::ControlPointsComponent>().Points;
		for (auto& point : points)
			m_State.ObjectsToShow.insert(point);
	}
	m_State.ShouldUpdateVisibility = true;
}

