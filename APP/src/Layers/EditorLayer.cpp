#include "EditorLayer.h"
#include "core/ImGui/ScopedDisable.h"
#include "Tools/EditorConstants.h"

EditorLayer::EditorLayer(float aspectRatio)
{
	m_CameraController = std::make_shared<ar::CameraController>(EditorCamera::FOV, aspectRatio,
		EditorCamera::NearPlane, EditorCamera::FarPlane, EditorCamera::ArcballRadius);
	m_Scene = std::make_shared<ar::Scene>();

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

	if (m_State.ShouldDeleteObjects)
	{
		DeleteObjects();
		m_State.ClearDeleteState();
	}

	if (m_State.ShouldRenameObject)
	{
		m_State.ObjectToRename.SetName(m_State.RenameBuffer);
		m_State.ClearRenameState();
	}

	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
	ar::RenderCommand::Clear();

	m_Scene->OnUpdate(m_CameraController->GetCamera());
	m_Cursor.Render(m_CameraController, m_ViewportFramebuffer->GetHeight());

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
	RenderMainMenu();
	RenderViewport();
	RenderStatsWindow();
	RenderSceneHierarchy();
	RenderInspectorWindow();
	RenderCursorControls();

	// Modals
	RenderDeleteModal();
	RenderRenameModal();
	RenderErrorModal();
}

bool EditorLayer::OnMouseMoved(ar::MouseMovedEvent& event)
{
	return false;
}

bool EditorLayer::OnMouseScrolled(ar::MouseScrolledEvent& event)
{
	return false;
}

void EditorLayer::RenderStatsWindow()
{
	ImGui::Begin("Stats");
	ImGui::TextWrapped("Current entity count: %d", m_Scene->GetEntityCount());
	ImGui::End();
}

void EditorLayer::RenderMainMenu()
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
			if (ImGui::MenuItem("Point")) { AddPoint(); }
			if (ImGui::BeginMenu("Mesh"))
			{
				if (ImGui::MenuItem("Torus")) { AddObject(ar::ObjectType::TORUS); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Curve"))
			{
				if (ImGui::MenuItem("Chain")) { AddObject(ar::ObjectType::CHAIN); }
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

void EditorLayer::RenderViewport()
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

	// cursor placement
	if (ar::Input::IsMouseButtonPressed(AR_MOUSE_BUTTON_RIGHT) && ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
		PlaceCursor();
	
	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::RenderSceneHierarchy()
{
	ImGui::Begin("Scene Hierarchy");

	for (auto entity : m_Scene->m_Registry.view<entt::entity>(entt::exclude<ar::VirtualTagComponent>))
	{
		ar::Entity e{ entity, m_Scene.get() };
		DrawTreeNode(e);
	}

	ImGui::End();
}

void EditorLayer::RenderInspectorWindow()
{
	ImGui::Begin("Inspector");

	auto object = GetLastSelected();
	if (object)
		ar::ComponentInspector::ShowInspector(object);

	ImGui::End();
}

void EditorLayer::RenderCursorControls()
{
	ImGui::Begin("Cursor");

	ar::mat::Vec3 position = m_Cursor.GetPosition();
	if (ImGui::DragFloat3("Cursor position", position.Data(), 0.1f, -20.0f, 20.0f))
	{
		m_Cursor.SetPosition(position);
	}

	ImGui::End();
}

void EditorLayer::DrawTreeNode(ar::Entity& object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull;

	if (!object.HasComponent<ar::ControlPointsComponent>())
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		if (object.HasComponent<ar::SelectedTagComponent>())
			flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)object.GetID(), flags, "%s", object.GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			if (!ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
				ClearSelection();
			if (object.HasComponent<ar::SelectedTagComponent>())
				DeselectObject(object);
			else
				SelectObject(object);
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Rename"))
			{
				m_State.ObjectToRename = object;
				m_State.ShowRenameModal = true;
			}
			if (ImGui::MenuItem("Delete"))
			{
				m_State.ObjectsToDelete.push_back(object);
				m_State.ShowDeleteModal = true;
			}
			{
				ar::ScopedDisable disableSelectionDelete(
					m_State.SelectedObjects.empty()
					|| !object.HasComponent<ar::SelectedTagComponent>());
				if (ImGui::MenuItem("Delete Selected"))
				{
					m_State.ShowDeleteModal = true;
					m_State.ObjectsToDelete.insert(
						m_State.ObjectsToDelete.end(),
						m_State.SelectedObjects.begin(),
						m_State.SelectedObjects.end()
					);
				}
			}
			ImGui::EndPopup();
		}
	}
	else
	{
		if (object.HasComponent<ar::SelectedTagComponent>())
			flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)object.GetID(), flags, "%s", object.GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			if (!ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
				ClearSelection();
			if (object.HasComponent<ar::SelectedTagComponent>())
				DeselectObject(object);
			else
				SelectObject(object);
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Rename"))
			{
				m_State.ObjectToRename = object;
				m_State.ShowRenameModal = true;
			}
			if (ImGui::MenuItem("Delete"))
			{
				m_State.ObjectsToDelete.push_back(object);
				m_State.ShowDeleteModal = true;
			}
			{
				ar::ScopedDisable disableSelectionDelete(
					m_State.SelectedObjects.empty()
					|| !object.HasComponent<ar::SelectedTagComponent>());
				if (ImGui::MenuItem("Delete Selected"))
				{
					m_State.ShowDeleteModal = true;
					m_State.ObjectsToDelete.insert(
						m_State.ObjectsToDelete.end(),
						m_State.SelectedObjects.begin(),
						m_State.SelectedObjects.end()
					);
				}
			}
			ImGui::EndPopup();
		}

		if (node_open)
		{
			for (auto& point : object.GetComponent<ar::ControlPointsComponent>().Points)
			{
				DrawTreeChildNode(object, point);
			}
			ImGui::TreePop();
		}
	}
}

void EditorLayer::DrawTreeChildNode(ar::Entity& parent, ar::Entity& object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull |
		ImGuiTreeNodeFlags_Leaf |
		ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (object.HasComponent<ar::SelectedTagComponent>())
		flags |= ImGuiTreeNodeFlags_Selected;

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)object.GetID(), flags, "%s", object.GetName().c_str());

	if (ImGui::IsItemClicked())
	{
		if (!ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
			ClearSelection();
		if (object.HasComponent<ar::SelectedTagComponent>())
			DeselectObject(object);
		else
			SelectObject(object);
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Rename"))
		{
			m_State.ObjectToRename = object;
			m_State.ShowRenameModal = true;
		}
		if (ImGui::MenuItem("Delete"))
		{
			m_State.ObjectsToDelete.push_back(object);
			m_State.ShowDeleteModal = true;
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Detach"))
		{
			DetachFromParent(object, parent);
		}
		ImGui::EndPopup();
	}
}

void EditorLayer::RenderDeleteModal()
{
	const char* popupName = "Delete?";
	
	if (m_State.ShowDeleteModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowDeleteModal = false;
	}
	
	CenterNextPopup();
	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::string message = "Are you sure you want to delete "
			+ std::to_string(m_State.ObjectsToDelete.size())
			+ " object(s)?";
		ImGui::TextWrapped(message.c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_State.ShouldDeleteObjects = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_State.ClearDeleteState();
		}
		ImGui::EndPopup();
	}
}

void EditorLayer::RenderRenameModal()
{
	const char* popupName = "Rename?";

	if (m_State.ShowRenameModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowRenameModal = false;
	}
	
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputTextWithHint("##new_name", "Enter new name here", m_State.RenameBuffer, IM_ARRAYSIZE(m_State.RenameBuffer));

		std::string message = "Are you sure you want to change the name?";
		ImGui::TextWrapped(message.c_str());
		{
			ar::ScopedDisable disabledOK(m_State.RenameBuffer[0] == '\0');
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				m_State.ShouldRenameObject = true;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.ClearRenameState();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void EditorLayer::RenderErrorModal()
{
	const char* popupName = "Error";

	if (m_State.ShowErrorModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowErrorModal = false;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped(m_State.ErrorMessages[0].c_str());
		
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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
			break;
		}
		case ar::ObjectType::CHAIN:
		{
			AddChain();
			break;
		}
	}
}

void EditorLayer::DeleteObjects()
{
	for (auto& object : m_State.ObjectsToDelete)
	{
		if (object.HasComponent<ar::SelectedTagComponent>())
			DeselectObject(object);
		if (object.HasComponent<ar::PointTagComponent>())
		{
			// remove from all point composites
			auto view = m_Scene->m_Registry.view<ar::ControlPointsComponent>();
			for (auto [entity, cp] : view.each())
			{
				cp.Points.erase(std::remove(cp.Points.begin(), cp.Points.end(), object), cp.Points.end());
			}
		}
		m_Scene->DestroyEntity(object);
	}
	m_State.ObjectsToDelete.clear();
}

void EditorLayer::MarkObjectDelete(ar::Entity object)
{
	m_State.ObjectsToDelete.push_back(object);
}

void EditorLayer::MarkMultipleObjectsDelete(std::vector<ar::Entity> objects)
{
	for (auto& object : objects)
		MarkObjectDelete(object);
}

void EditorLayer::DetachFromParent(ar::Entity object, ar::Entity parent)
{
	if (parent.HasComponent<ar::ChainTagComponent>())
		DetachFromChain(object, parent);
}

void EditorLayer::PlaceCursor()
{
	// get cursor position inside the viewport window
	ImVec2 viewport_pos = ImGui::GetItemRectMin();
	ImVec2 viewport_size = ImGui::GetItemRectSize();
	ImVec2 mouse_pos = ImGui::GetIO().MousePos;

	float xPos = mouse_pos.x - viewport_pos.x;
	float yPos = mouse_pos.y - viewport_pos.y;

	if (xPos < 0 || yPos < 0 ||
		xPos >= viewport_size.x ||
		yPos >= viewport_size.y)
	{
		return;
	}

	// convert to NDC
	float xNDC = (2.0f * xPos) / viewport_size.x - 1.0f;
	float yNDC = 1.0f - (2.0f * yPos) / viewport_size.y;
	auto rayClip = ar::mat::Vec4(xNDC, yNDC, -1.0f, 1.0f);

	// convert to camera space
	auto rayCamera = m_CameraController->GetCamera()->GetInvProjection() * rayClip;
	rayCamera = ar::mat::Vec4(rayCamera.x, rayCamera.y, -1.0f, 0.0f);

	// convert to world space
	auto direction = ar::mat::Normalize(ar::mat::ToVec3(m_CameraController->GetCamera()->GetInvView() * rayCamera));
	
	ar::mat::Vec3 origin = ar::mat::ToVec3(m_CameraController->GetPosition());
	ar::mat::Vec3 planeNormal = ar::mat::ToVec3(m_CameraController->GetCamera()->GetForward());
	float t = -((ar::mat::Dot(origin, planeNormal)) / ar::mat::Dot(direction, planeNormal));
	auto cursorPos = origin + direction * t;
	m_Cursor.SetPosition(cursorPos);
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
	trc.Translation = m_Cursor.GetPosition();

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

void EditorLayer::AddChain()
{
	if (m_State.SelectedPoints.size() < 2)
	{
		m_State.ShowErrorModal = true;
		m_State.ErrorMessages.emplace_back("To create a chain, you need at least 2 points selected in the scene hierarchy!");
		return;
	}

	auto entity = m_Scene->CreateEntity("Chain");
	entity.AddComponent<ar::ChainTagComponent>();
	entity.AddComponent<ar::ControlPointsComponent>(m_State.SelectedPoints);
}

void EditorLayer::AddPoint()
{
	auto entity = m_Scene->CreateEntity("Point");
	
	// Point tag component
	entity.AddComponent<ar::PointTagComponent>();

	// Transform component
	auto& trc = entity.AddComponent<ar::TransformComponent>();
	trc.Translation = m_Cursor.GetPosition();
	trc.IsRotationEnabled = false;
	trc.IsScaleEnabled = false;
}

void EditorLayer::DetachFromChain(ar::Entity object, ar::Entity parent)
{
	auto& points = parent.GetComponent<ar::ControlPointsComponent>().Points;
	points.erase(std::remove(points.begin(), points.end(), object), points.end());
	if (points.size() < 2)
		MarkObjectDelete(parent);
}

std::vector<ar::mat::Mat4> EditorLayer::GetPointModelMatrices()
{
	std::vector<ar::mat::Mat4> matrices;
	auto view = m_Scene->m_Registry.view<ar::TransformComponent, ar::PointTagComponent>();

	matrices.reserve(view.size_hint());
	for (auto [entity, transform] :view.each())
	{
		matrices.push_back(transform.ModelMatrix);
	}
	return matrices;
}

void EditorLayer::SelectObject(ar::Entity object)
{
	object.AddComponent<ar::SelectedTagComponent>();
	m_State.SelectedObjects.push_back(object);

	if (object.HasComponent<ar::PointTagComponent>())
		m_State.SelectedPoints.push_back(object);
}

void EditorLayer::DeselectObject(ar::Entity object)
{
	auto& objs = m_State.SelectedObjects;
	objs.erase(std::remove(objs.begin(), objs.end(), object), objs.end());

	if (object.HasComponent<ar::PointTagComponent>())
	{
		auto& pts = m_State.SelectedPoints;
		pts.erase(std::remove(pts.begin(), pts.end(), object), pts.end());
	}
	object.RemoveComponent<ar::SelectedTagComponent>();
}

ar::Entity EditorLayer::GetLastSelected()
{
	if (m_State.SelectedObjects.empty())
		return { entt::null, nullptr };
	return m_State.SelectedObjects.back();
}

void EditorLayer::ClearSelection()
{

}

