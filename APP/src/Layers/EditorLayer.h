#pragma once
#include <ARCAD.h>
#include <ARMAT.h>
#include "core/ImGui/ComponentInspector.h"
#include "Tools/EditorCursor.h"
#include "Tools/ImGuiUtils.h"
#include "Tools/EditorState.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Layers/EditorUI.h"
#include "EditorSceneController.h"

class EditorLayer : public ar::Layer
{

public:
	EditorLayer(float aspectRatio);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(ar::Event& event) override;
	void OnImGuiRender() override;
	
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

	// State
	EditorState m_State;
	// EditorUI
	EditorUI m_UI;
	// EditorSceneController
	EditorSceneController m_SceneController;

	std::vector<ar::mat::Mat4> GetPointModelMatrices();

};


