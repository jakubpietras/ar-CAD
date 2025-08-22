#pragma once
#include "ARCAD.h"
#include "EditorState.h"

class SceneHierarchyPanel
{
public:
	SceneHierarchyPanel(EditorState& state);
	void Render();
	void SetContext(ar::Ref<ar::Scene> scene);

private:
	void DrawParentNode(ar::Entity object);
	void DrawChildNode(ar::Entity child, ar::Entity parent);

	void DrawEntityContextMenu(ar::Entity& object, bool allowDeleteSelected, bool allowAttachment);
	void DrawLinkContextMenu(ar::Entity& child, ar::Entity& parent);

	void RequestObjectSelect(ar::Entity object, bool add = false);
	void RequestObjectDeselect(ar::Entity object);

	EditorState& m_State;
	ar::Ref<ar::Scene> m_Scene;

	bool m_ShowUnselectedPoints = false;

};