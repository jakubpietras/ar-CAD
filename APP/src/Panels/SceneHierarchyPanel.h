#pragma once
#include "ARCAD.h"
#include "Tools/EditorState.h"

class SceneHierarchyPanel
{
public:
	SceneHierarchyPanel(EditorState& state);
	void Render();
	void SetContext(ar::Ref<ar::Scene> scene);

private:
	void DrawParentNode(ar::Entity object);
	void DrawChildNode(ar::Entity child, ar::Entity parent);

	void DrawEntityContextMenu(ar::Entity& object, bool allowDeleteSelected);
	void DrawLinkContextMenu(ar::Entity& child, ar::Entity& parent);

	void SelectObject(ar::Entity object);
	void DeselectObject(ar::Entity object);

	EditorState& m_State;
	ar::Ref<ar::Scene> m_Scene;

};