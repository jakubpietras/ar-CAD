#include "EditorLayer.h"

static const float s_InitFOV = 45.0f;
static const float s_InitNearPlane = 0.1f;
static const float s_InitFarPlane = 100.0f;
static const float s_InitArcballRadius = 5.0f;

EditorLayer::EditorLayer(float aspectRatio)
{
	m_CameraController = std::make_shared<ar::CameraController>(s_InitFOV, aspectRatio,
		s_InitNearPlane, s_InitFarPlane, s_InitArcballRadius);
	m_CubeShader = std::shared_ptr<ar::Shader>(
		ar::Shader::Create("resources/shaders/OpenGL/default.vert",
			"resources/shaders/OpenGL/default.frag"));
	
	std::vector<ar::VertexPositionColor> cubeVerts = {
		// position				color
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},

		{{-0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}},

		{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},

		{{0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}},
		{{0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 1.0f}},
		{{0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 1.0f}},
		{{0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 1.0f}},
		{{0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}},
		{{0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},

		{{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f, 0.0f}}
	};

	m_Cube = std::shared_ptr<ar::VertexArray>(ar::VertexArray::Create());
	m_Cube->AddVertexBuffer(std::shared_ptr<ar::VertexBuffer>(ar::VertexBuffer::Create(cubeVerts)));
}

void EditorLayer::OnAttach() { }

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_CameraController->OnUpdate();
	auto cam = m_CameraController->GetCamera();

	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.5f, 0.5f, 1.0f, 1.0f));
	ar::RenderCommand::ToggleDepthTest(true);
	ar::RenderCommand::Clear();
	ar::Renderer::BeginScene();
	m_CubeShader->SetMat4("u_VP", cam->GetVP());
	ar::Renderer::Submit(m_CubeShader, m_Cube);
	ar::Renderer::EndScene();
}

void EditorLayer::OnEvent(ar::Event& event)
{
	m_CameraController->OnEvent(event);
	ar::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<ar::MouseMovedEvent>(AR_BIND_EVENT_FN(EditorLayer::OnMouseMoved));
	dispatcher.Dispatch<ar::MouseScrolledEvent>(AR_BIND_EVENT_FN(EditorLayer::OnMouseScrolled));
}

void EditorLayer::OnImGuiRender() { }

bool EditorLayer::OnMouseMoved(ar::MouseMovedEvent& event)
{
	return false;
}

bool EditorLayer::OnMouseScrolled(ar::MouseScrolledEvent& event)
{
	return false;
}

