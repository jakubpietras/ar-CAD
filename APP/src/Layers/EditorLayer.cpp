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
	m_GridShader = std::shared_ptr<ar::Shader>(
		ar::Shader::Create("resources/shaders/OpenGL/grid.vert",
			"resources/shaders/OpenGL/grid.frag"));
	m_CommandQueue = std::make_unique<ar::CommandQueue>();
	m_Scene = std::make_shared<ar::Scene>();
	m_SceneHierarchyPanel = SceneHierarchyPanel(m_Scene, m_CommandQueue);

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

	std::vector<ar::InstancedFloat3> cubeOffsets
	{
		{{0.0f, 0.0f, 0.0f}},
		{{0.5f, 2.0f, 0.0f}},
		{{-1.0f, 4.0f, 0.5f}}
	};

	std::vector<ar::InstancedMat4> modelMatrices
	{
		{ar::mat::RotationMatrix(10.0f, 10.f, 0.f)},
		{ ar::mat::RotationMatrix(20.0f, 0.f, 20.f) },
		{ ar::mat::RotationMatrix(10.0f, 40.f, -15.f) }
	};

	m_Cube = std::shared_ptr<ar::VertexArray>(ar::VertexArray::Create());
	m_Cube->AddVertexBuffer(std::shared_ptr<ar::VertexBuffer>(ar::VertexBuffer::Create(cubeVerts)));
	m_Cube->AddVertexBuffer(std::shared_ptr<ar::VertexBuffer>(ar::VertexBuffer::Create(cubeOffsets)));
	m_Cube->AddVertexBuffer(std::shared_ptr<ar::VertexBuffer>(ar::VertexBuffer::Create(modelMatrices)));

	ar::FramebufferDesc fbDesc;
	// todo: parameterize
	fbDesc.Height = 1080;
	fbDesc.Width = 1920;
	m_Framebuffer = std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create(fbDesc));

	m_MenuIcon = std::unique_ptr<ar::Texture>(ar::Texture::Create("resources/icons/logo.png"));

}

void EditorLayer::OnAttach() { }

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_CameraController->OnUpdate();
	m_Framebuffer->Bind();
	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.25f, 0.25f, 0.25f, 1.0f));
	ar::RenderCommand::Clear();

	/*ar::Renderer::BeginScene();
	RenderCube();*/
	//RenderGrid();
	m_Scene->RenderScene(m_CameraController->GetCamera());
	//ar::Renderer::EndScene();

	m_Framebuffer->Unbind();
}

void EditorLayer::OnEvent(ar::Event& event)
{
	m_CameraController->OnEvent(event);
	ar::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<ar::MouseMovedEvent>(AR_BIND_EVENT_FN(EditorLayer::OnMouseMoved));
	dispatcher.Dispatch<ar::MouseScrolledEvent>(AR_BIND_EVENT_FN(EditorLayer::OnMouseScrolled));
}

void EditorLayer::OnImGuiRender() 
{
	ShowMenu();
	ShowViewport();
	ShowStats();
	m_SceneHierarchyPanel.OnImGuiRender();
}

bool EditorLayer::OnMouseMoved(ar::MouseMovedEvent& event)
{
	return false;
}

bool EditorLayer::OnMouseScrolled(ar::MouseScrolledEvent& event)
{
	return false;
}

void EditorLayer::RenderGrid()
{
	m_GridShader->SetMat4("u_VP", m_CameraController->GetCamera()->GetVP());
	ar::Renderer::Submit(ar::Primitive::Triangle, m_GridShader, 6);
}

void EditorLayer::RenderCube()
{
	m_CubeShader->SetMat4("u_VP", m_CameraController->GetCamera()->GetVP());
	ar::Renderer::Submit(ar::Primitive::Triangle, m_CubeShader, m_Cube, 3);
}

void EditorLayer::ShowStats()
{
	ImGui::Begin("Stats");
	ImGui::TextWrapped("Current entity count: %d", m_Scene->GetEntityCount());
	ImGui::End();
}

void EditorLayer::ShowMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::Image(
			(ImTextureID)(uintptr_t)m_MenuIcon->GetID(),
			ImVec2(48, 16),
			ImVec2(0, 1), ImVec2(1, 0)
		);

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "CTRL+S")) {}
			if (ImGui::MenuItem("Load", "CTRL+L")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "CTRL+Q")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z", false, m_CommandQueue->CanUndo())) { UndoLastCommand(); }
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, m_CommandQueue->CanRedo())) { RedoLastCommand(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::BeginMenu("Mesh"))
			{
				if (ImGui::MenuItem("Torus")) { AddObject(ar::ObjectType::TORUS); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Curve"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Surface"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenu();

		}

		ImGui::EndMainMenuBar();
	}
}

void EditorLayer::ShowViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	ar::Application::Get().ImGuiBlockEvents(!ImGui::IsWindowHovered());
	
	auto viewportSize = ImGui::GetContentRegionAvail();
	
	if (viewportSize.x != m_ViewportSize.first || viewportSize.y != m_ViewportSize.second)
	{
		m_Framebuffer->Resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
		m_ViewportSize = { viewportSize.x, viewportSize.y };
		m_CameraController->SetAspectRatio(viewportSize.x / viewportSize.y);
	}

	ImGui::Image(
		(ImTextureID)(uintptr_t)m_Framebuffer->GetColorAttachment(),
		viewportSize,
		ImVec2(0, 1), ImVec2(1, 0)
	);
	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::AddObject(ar::ObjectType type)
{
	switch (type)
	{
		case ar::ObjectType::TORUS:
		{
			ar::TorusDesc desc;
			auto command = std::make_unique<ar::AddTorusCommand>(desc, m_Scene);
			m_CommandQueue->Execute(std::move(command));
		}
	}
}

