#include "EditorLayer.h"
#include "core/ImGui/ScopedDisable.h"

static const float s_InitFOV = 45.0f;
static const float s_InitNearPlane = 0.1f;
static const float s_InitFarPlane = 100.0f;
static const float s_InitArcballRadius = 5.0f;

EditorLayer::EditorLayer(float aspectRatio)
{
	m_CameraController = std::make_shared<ar::CameraController>(s_InitFOV, aspectRatio,
		s_InitNearPlane, s_InitFarPlane, s_InitArcballRadius);
	m_Scene = std::make_shared<ar::Scene>();
	m_Selection = {};
	m_ObjectsToDelete = {};

	ar::FramebufferDesc fbDesc;
	// todo: parameterize
	fbDesc.Height = 1080;
	fbDesc.Width = 1920;
	m_ViewportFramebuffer = std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create(fbDesc));

	m_MenuIcon = std::unique_ptr<ar::Texture>(ar::Texture::Create("resources/icons/logo.png"));
}

void EditorLayer::OnAttach() 
{
	ar::ShaderLib::Init();
}

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_CameraController->OnUpdate();
	m_ViewportFramebuffer->Bind();

	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
	ar::RenderCommand::Clear();
	m_Scene->OnUpdate(m_CameraController->GetCamera());

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
	DrawDeleteModal();
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
			if (ImGui::MenuItem("Rename"))
			{
				// todo: open rename dialog
			}
			if (ImGui::MenuItem("Delete"))
			{	
				m_ShouldOpenDeleteModal = true;
				m_ObjectsToDelete.push_back(object);
			}
			{
				ar::ScopedDisable disableSelectionDelete(
					m_Selection.SelectedObjects.empty() 
					|| !object.HasComponent<ar::SelectedTagComponent>());

				if (ImGui::MenuItem("Delete Selected"))
				{
					m_ShouldOpenDeleteModal = true;
					m_Selection.ShouldDelete = true;
				}
			}
			
			
			ImGui::EndPopup();
		}
	}
}

void EditorLayer::DrawDeleteModal()
{
	const char* popupName = "Delete?";
	
	if (m_ShouldOpenDeleteModal)
	{
		ImGui::OpenPopup(popupName);
		m_ShouldOpenDeleteModal = false;
	}
	

	// individual item
	if (m_Selection.ShouldDelete)
	{
		if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			std::string message = "Are you sure you want to delete " 
				+ std::to_string(m_Selection.SelectedObjects.size()) 
				+ " object(s)?";
			ImGui::TextWrapped(message.c_str());

			// centering
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				DeleteMultipleObjects(m_Selection.SelectedObjects);
				m_Selection.ShouldDelete = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_Selection.ShouldDelete = false;
				m_ObjectsToDelete.clear();
			}
			ImGui::EndPopup();
		}
	}
	else
	{
		if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::TextWrapped("Are you sure you want to delete this object?");
			// centering
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				DeleteMultipleObjects(m_ObjectsToDelete);
				m_ObjectsToDelete.clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_ObjectsToDelete.clear();
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
			AddTorus(desc);
		}
	}
}

void EditorLayer::DeleteObject(ar::Entity object)
{
	if (object.HasComponent<ar::SelectedTagComponent>())
		DeselectObject(object);
	m_Scene->DestroyEntity(object);
}

void EditorLayer::DeleteMultipleObjects(std::vector<ar::Entity> objects)
{
	for (auto& object : objects)
		DeleteObject(object);
}

void EditorLayer::SelectObject(ar::Entity object)
{
	object.AddComponent<ar::SelectedTagComponent>();
	if (object.HasComponent<ar::PointTagComponent>())
		m_Selection.SelectedPoints.push_back(object);
	m_Selection.SelectedObjects.push_back(object);
	m_Selection.CurrentlySelected = object;
}

void EditorLayer::DeselectObject(ar::Entity object)
{
	if (m_Selection.CurrentlySelected == object)
		m_Selection.CurrentlySelected = { entt::null, nullptr };

	object.RemoveComponent<ar::SelectedTagComponent>();
	if (object.HasComponent<ar::PointTagComponent>())
	{
		auto& v = m_Selection.SelectedPoints;
		v.erase(std::remove(v.begin(), v.end(), object), v.end());
	}
	auto& objs = m_Selection.SelectedObjects;
	objs.erase(std::remove(objs.begin(), objs.end(), object), objs.end());
}

void EditorLayer::DeselectAll()
{
	m_Scene->m_Registry.clear<ar::SelectedTagComponent>();
	m_Selection.SelectedPoints.clear();
	m_Selection.CurrentlySelected = { entt::null, nullptr };
}

void EditorLayer::AddTorus(ar::TorusDesc desc)
{
	auto entity = m_Scene->CreateEntity("Torus");

	// Torus component
	auto& tc = entity.AddComponent<ar::TorusComponent>();
	tc.Description = desc;
	tc.Vertices = ar::TorusUtils::GenerateTorusVertices(desc);
	tc.Edges = ar::TorusUtils::GenerateTorusEdges(desc);

	// Transform component
	auto& trc = entity.AddComponent<ar::TransformComponent>();
	trc.Translation = m_Scene->GetCursorPos();

	// Mesh (render) component
	auto& mc = entity.AddComponent<ar::MeshComponent>();

	// Primitive
	mc.RenderPrimitive = ar::Primitive::LineLoop;

	// VertexArray
	mc.VertexArray = ar::Scope<ar::VertexArray>(ar::VertexArray::Create());
	mc.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(tc.Vertices)));
	auto indexBuffers = ar::IndexBuffer::Create(tc.Edges);
	for (auto& ib : indexBuffers)
		mc.VertexArray->AddIndexBuffer(ib);

	// Shader
	mc.Shader = ar::ShaderLib::Get("Basic");
}

