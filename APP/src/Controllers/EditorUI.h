#pragma once
#include <imgui.h>
#include "Panels/SceneHierarchyPanel.h"
#include "core/Drawing/PaintSurface.h"

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
	ar::PaintSurface m_Paint;

	ar::mat::Vec2 GetClickPosition();

	void RenderGroupTransformControls();
	void RenderPickingBox();
	void RenderStatsWindow();
	void RenderMainMenu();
	void RenderInspectorWindow();
	void RenderCursorControls();
	void RenderViewport(ar::Ref<ar::Framebuffer> mainFB);
	void RenderAddObjectPopup();
	void RenderPaintWindow(ar::PaintSurface& paintSurface);

	void RenderAddMenu();
	bool RenderRectangleControls();
	bool RenderCylinderControls();

	void OpenImportDialog();
	void OpenExportDialog();
	std::string OpenFileDialog();

	// modal
	void RenderDeleteModal();
	void RenderRenameModal();
	void RenderErrorModal();
	void RenderDetachModal();
	void RenderAttachModal();
	void RenderAddSurfaceC0Modal();
	void RenderAddSurfaceC2Modal();
	void RenderAddGregoryModal();
	void RenderCollapseModal();

	void RequestAddObject(ar::ObjectType type);
};