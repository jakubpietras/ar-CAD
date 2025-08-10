#pragma once
#include <imgui.h>
#include "Tools/EditorCursor.h"
#include "Panels/SceneHierarchyPanel.h"

class EditorUI
{
public:
	EditorUI(EditorState& state, ar::Ref<ar::Scene> scene);

	void Render();
	void RenderCursor(ar::Ref<ar::CameraController> cameraController);
	const ar::Ref<ar::Framebuffer>& GetFramebuffer();

private:
	EditorState& m_State;
	ar::Ref<ar::Scene> m_Scene;
	EditorCursor m_Cursor;
	SceneHierarchyPanel m_SceneHierarchyPanel;
	ar::Scope<ar::Texture> m_MenuIcon;
	ar::Ref<ar::Framebuffer> m_ViewportFramebuffer;
	
	void RenderStatsWindow();
	void RenderMainMenu();
	void RenderInspectorWindow();
	void RenderCursorControls();
	void RenderViewport();

	// modal
	void RenderDeleteModal();
	void RenderRenameModal();
	void RenderErrorModal();
	void RenderDetachModal();


	void RequestAddObject(ar::ObjectType type);
	void RequestPlaceCursor();
};