#pragma once
#include <ARCAD.h>
#include <ARMAT.h>
#include "core/ImGui/ComponentInspector.h"
#include "Tools/EditorCursor.h"
#include "Tools/ImGuiUtils.h"
#include "Tools/EditorState.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Layers/EditorUI.h"

class EditorLayer : public ar::Layer
{

public:
	EditorLayer(float aspectRatio);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(ar::Event& event) override;
	void OnImGuiRender() override;
	
	// Commands
	void MarkObjectDelete(ar::Entity object);
	void MarkMultipleObjectsDelete(std::vector<ar::Entity> objects);
	void AddObject(ar::ObjectType type);
	void DeleteObjects();
	void DetachFromParent(ar::Entity object, ar::Entity parent);

	// Selection
	void SelectObject(ar::Entity object);
	void DeselectObject(ar::Entity object);
	ar::Entity GetLastSelected();
	void ClearSelection();

	// cursor
	void PlaceCursor();
	
private:
	
	// Logic
	ar::Ref<ar::Scene> m_Scene;
	ar::Ref<ar::CameraController> m_CameraController;
	std::pair<float, float> m_ViewportSize;

	// State
	EditorState m_State;
	// EditorUI
	EditorUI m_UI;

	// Panels
	SceneHierarchyPanel m_SceneHierarchyPanel;
	// Cursor
	EditorCursor m_Cursor;
	
	// Rendering
	ar::Ref<ar::Framebuffer> m_ViewportFramebuffer;

	// Textures
	ar::Scope<ar::Texture> m_MenuIcon;

	// Addition
	void AddTorus(ar::TorusDesc desc);
	void AddChain();
	void AddPoint();

	// Detachment
	void DetachFromChain(ar::Entity object, ar::Entity parent);

	std::vector<ar::mat::Mat4> GetPointModelMatrices();

};


