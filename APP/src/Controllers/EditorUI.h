#pragma once
#include <imgui.h>
#include "Panels/SceneHierarchyPanel.h"

class EditorUI
{
public:
	EditorUI(EditorState& state, ar::Ref<ar::Scene> scene);

	void Render(ar::Ref<ar::Framebuffer> mainFB);

private:
	EditorState& m_State;
	ar::Ref<ar::Scene> m_Scene;
	SceneHierarchyPanel m_SceneHierarchyPanel;
	ar::Scope<ar::Texture> m_MenuIcon;
	
	ar::mat::Vec2 GetClickPosition();

	void RenderGroupTransformControls();
	void RenderPickingBox();
	void RenderStatsWindow();
	void RenderMainMenu();
	void RenderInspectorWindow();
	void RenderCursorControls();
	void RenderViewport(ar::Ref<ar::Framebuffer> mainFB);
	void RenderAddObjectPopup();
	void RenderAddSurfacePopup();

	void RenderAddMenu();

	// modal
	void RenderDeleteModal();
	void RenderRenameModal();
	void RenderErrorModal();
	void RenderDetachModal();
	void RenderAttachModal();

	void RequestAddObject(ar::ObjectType type);
};