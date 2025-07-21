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

private:
	std::shared_ptr<ar::CameraController> m_CameraController;

	std::shared_ptr<ar::VertexArray> m_Cube;
	std::shared_ptr<ar::Shader> m_CubeShader;
};


