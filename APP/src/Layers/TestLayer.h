#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

class TestLayer : public ar::Layer
{
public:
	TestLayer();
	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(ar::Event& event) override;

	// Event handlers:
	bool OnKeyPressed(ar::KeyPressedEvent& event);
	bool OnMouseMoved(ar::MouseMovedEvent& event);
	bool OnMouseScrolled(ar::MouseScrolledEvent& event);

private:
	std::shared_ptr<ar::VertexBuffer> vbo;
	std::shared_ptr<ar::VertexArray> vao;
	std::shared_ptr<ar::PerspectiveCamera> camera;
	std::shared_ptr<ar::Shader> shader;
};

