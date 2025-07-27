#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

#include "Panels/SceneHierarchyPanel.h"

class EditorLayer : public ar::Layer
{

public:
	EditorLayer(float aspectRatio);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(ar::Event& event) override;
	void OnImGuiRender() override;

	// Event handlers
	bool OnMouseMoved(ar::MouseMovedEvent& event);
	bool OnMouseScrolled(ar::MouseScrolledEvent& event);

	// Render functions
	void RenderGrid();
	void RenderCube();

	// GUI functions
	void ShowStats();
	void ShowMenu();
	void ShowViewport();

	// Commands
	void AddObject(ar::ObjectType type);
	inline void UndoLastCommand() { m_CommandQueue->Undo(); }
	inline void RedoLastCommand() { m_CommandQueue->Redo(); }

private:
	bool m_ViewportFocused = false;

	std::pair<float, float> m_ViewportSize;

	std::shared_ptr<ar::CameraController> m_CameraController;
	std::shared_ptr<ar::Framebuffer> m_Framebuffer;

	std::shared_ptr<ar::VertexArray> m_Cube;
	std::shared_ptr<ar::Shader> m_CubeShader, m_GridShader;

	std::unique_ptr<ar::Texture> m_MenuIcon;

	std::shared_ptr<ar::Scene> m_Scene;
	std::shared_ptr<ar::CommandQueue> m_CommandQueue;

	SceneHierarchyPanel m_SceneHierarchyPanel;

};


