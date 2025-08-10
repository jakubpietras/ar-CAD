#pragma once
#include <ARCAD.h>
#include <ARMAT.h>
#include "core/ImGui/ComponentInspector.h"
#include "Tools/EditorCursor.h"
#include "Tools/ImGuiUtils.h"
#include "EditorState.h"

#include "Controllers/EditorUI.h"
#include "Controllers/EditorSceneController.h"

class EditorLayer : public ar::Layer
{
public:
	EditorLayer(float aspectRatio);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(ar::Event& event) override;
	void OnImGuiRender() override;

private:
	ar::Ref<ar::Scene> m_Scene;
	EditorState m_State;
	EditorUI m_UI;
	EditorSceneController m_SceneController;
};


