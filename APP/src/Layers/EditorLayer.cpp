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
	m_Selection = {};

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
	m_ViewportFramebuffer = std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create(fbDesc));

	m_MenuIcon = std::unique_ptr<ar::Texture>(ar::Texture::Create("resources/icons/logo.png"));

}

void EditorLayer::OnAttach() { }

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_CameraController->OnUpdate();
	m_ViewportFramebuffer->Bind();
	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
	ar::RenderCommand::Clear();

	/*ar::Renderer::BeginScene();
	RenderCube();*/
	//RenderGrid();
	m_Scene->RenderScene(m_CameraController->GetCamera());
	//ar::Renderer::EndScene();

	m_ViewportFramebuffer->Unbind();
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
	ShowSceneHierarchy();
	ShowInspector();
}

bool EditorLayer::OnMouseMoved(ar::MouseMovedEvent& event)
{
	return false;
}

bool EditorLayer::OnMouseScrolled(ar::MouseScrolledEvent& event)
{
	return false;
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
		m_ViewportFramebuffer->Resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
		m_ViewportSize = { viewportSize.x, viewportSize.y };
		m_CameraController->SetAspectRatio(viewportSize.x / viewportSize.y);
	}

	ImGui::Image(
		(ImTextureID)(uintptr_t)m_ViewportFramebuffer->GetColorAttachment(),
		viewportSize,
		ImVec2(0, 1), ImVec2(1, 0)
	);
	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::ShowSceneHierarchy()
{
	ImGui::Begin("Scene Hierarchy");

	for (auto entity : m_Scene->m_Registry.view<entt::entity>(entt::exclude<ar::VirtualTagComponent>))
	{
		ar::Entity e{ entity, m_Scene.get() };
		DrawTreeNode(e);
	}

	ImGui::End();
}

void EditorLayer::ShowInspector()
{
	ImGui::Begin("Inspector");

	auto object = m_Selection.CurrentlySelected;
	if (object)
	{
		ar::ComponentInspector::ShowInspector(object);
	}

	ImGui::End();
}

void EditorLayer::DrawTreeNode(ar::Entity& object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull;

	if (object.HasComponent<ar::TorusComponent>())
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (object.HasComponent<ar::SelectedTagComponent>())
			flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)object.GetID(), flags, "%s", object.GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			if (!ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
				DeselectAll();
			if (object.HasComponent<ar::SelectedTagComponent>())
				DeselectObject(object);
			else
				SelectObject(object);
		}
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
			{	
			}
			if (ImGui::MenuItem("Rename"))
			{
			}
			ImGui::EndPopup();
		}
	}
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

void EditorLayer::SelectObject(ar::Entity object)
{
	object.AddComponent<ar::SelectedTagComponent>();
	if (object.HasComponent<ar::PointTagComponent>())
		m_Selection.SelectedPoints.push_back(object);
	m_Selection.CurrentlySelected = object;
}

void EditorLayer::DeselectObject(ar::Entity object)
{
	object.RemoveComponent<ar::SelectedTagComponent>();
	if (object.HasComponent<ar::PointTagComponent>())
	{
		auto& v = m_Selection.SelectedPoints;
		v.erase(std::remove(v.begin(), v.end(), object), v.end());
	}
	m_Selection.CurrentlySelected = { entt::null, nullptr };
}

void EditorLayer::DeselectAll()
{
	m_Scene->m_Registry.clear<ar::SelectedTagComponent>();
	m_Selection.SelectedPoints.clear();
	m_Selection.CurrentlySelected = { entt::null, nullptr };
}

