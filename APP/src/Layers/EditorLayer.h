#pragma once
#include <ARCAD.h>
#include <ARMAT.h>


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
	void ShowMenu();
	void ShowViewport();

private:

	std::pair<float, float> m_ViewportSize;

	std::shared_ptr<ar::CameraController> m_CameraController;
	std::shared_ptr<ar::Framebuffer> m_Framebuffer;

	std::shared_ptr<ar::VertexArray> m_Cube, m_Dummy;
	std::shared_ptr<ar::Shader> m_CubeShader, m_GridShader;

	std::unique_ptr<ar::Texture> m_MenuIcon;
};


