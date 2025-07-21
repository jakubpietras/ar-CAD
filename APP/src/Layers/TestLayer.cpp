#include "TestLayer.h"

TestLayer::TestLayer()
	: Layer("Example")
{
	camera = std::make_shared<ar::CameraController>(45.0f, 1920.0f / 1080.0f, 0.1f,
		100.0f, 5.0f);
	shader = std::shared_ptr<ar::Shader>(ar::Shader::Create(
		"resources/shaders/OpenGL/test.vert",
		"resources/shaders/OpenGL/test.frag"));

	auto va = ar::VertexArray::Create();
	vao = std::shared_ptr<ar::VertexArray>(va);
	std::vector<ar::VertexPositionColor> verts = {
		// position				color
		{ {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
		{ {0.5f, -0.5f, 0.0f} , {0.0f, 1.0f, 0.0f} },
		{ {0.0f,  0.5f, 0.0f} , {0.0f, 0.0f, 1.0f} }
	};
	auto vb = ar::VertexBuffer::Create(verts);
	vbo = std::shared_ptr<ar::VertexBuffer>(vb);
	va->AddVertexBuffer(vbo);
}

void TestLayer::OnUpdate()
{
	camera->OnUpdate();
	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.5f, 0.5f, 1.0f, 1.0f));
	ar::RenderCommand::Clear();

	ar::Renderer::BeginScene();
	shader->SetMat4("u_VP", camera->GetCamera()->GetVP());
	ar::Renderer::Submit(shader, vao);
	ar::Renderer::EndScene();
}

void TestLayer::OnImGuiRender() { }

void TestLayer::OnEvent(ar::Event& event)
{
	camera->OnEvent(event);
	ar::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<ar::KeyPressedEvent>(AR_BIND_EVENT_FN(TestLayer::OnKeyPressed));
	dispatcher.Dispatch<ar::MouseMovedEvent>(AR_BIND_EVENT_FN(TestLayer::OnMouseMoved));
	dispatcher.Dispatch<ar::MouseScrolledEvent>(AR_BIND_EVENT_FN(TestLayer::OnMouseScrolled));
}

bool TestLayer::OnKeyPressed(ar::KeyPressedEvent& event)
{
	return false;
}

bool TestLayer::OnMouseMoved(ar::MouseMovedEvent& event)
{
	return false;
}

bool TestLayer::OnMouseScrolled(ar::MouseScrolledEvent& event)
{
	return false;
}
