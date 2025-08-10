#include "EditorLayer.h"
#include "core/ImGui/ScopedDisable.h"
#include "Tools/EditorConstants.h"

EditorLayer::EditorLayer(float aspectRatio)
	: m_CameraController(std::make_shared<ar::CameraController>(
		EditorCameraConstants::FOV, aspectRatio,
		EditorCameraConstants::NearPlane, EditorCameraConstants::FarPlane,
		EditorCameraConstants::ArcballRadius)),
	m_Scene(std::make_shared<ar::Scene>()),
	m_ViewportFramebuffer(std::shared_ptr<ar::Framebuffer>(
		ar::Framebuffer::Create({ 1920, 1080 }))),
	m_MenuIcon(std::unique_ptr<ar::Texture>(
		ar::Texture::Create("resources/icons/logo.png"))),
	m_SceneHierarchyPanel(m_State),
	m_UI(m_State, m_Scene)
{
	m_SceneHierarchyPanel.SetContext(m_Scene);
}

void EditorLayer::OnAttach() 
{
	ar::ShaderLib::Init();
}

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_CameraController->OnUpdate();
	m_UI.GetFramebuffer()->Bind();

	// TODO: ProcessStateChanges(m_State)
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

	if (m_State.ShouldDetachPairs)
	{
		// todo: detach pairs
		m_State.ClearDetachState();
	}

	if (m_State.ViewportResized)
	{
		m_CameraController->SetAspectRatio(m_State.Viewport.Width / m_State.Viewport.Height);
		m_State.ViewportResized = false;
	}

	if (m_State.ShouldPlaceCursor)
	{
		PlaceCursor();
		m_State.ShouldPlaceCursor = false;
	}

	if (m_State.ShouldAddObject)
	{
		if (m_State.AddObjectType == ar::ObjectType::POINT)
			AddPoint();
		else
			AddObject(m_State.AddObjectType);
		m_State.ShouldAddObject = false;
	}


	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
	ar::RenderCommand::Clear();

	m_Scene->OnUpdate(m_CameraController->GetCamera());
	m_Cursor.Render(m_CameraController, m_ViewportFramebuffer->GetHeight());

	m_UI.GetFramebuffer()->Unbind();
}

void EditorLayer::OnEvent(ar::Event& event)
{
	m_CameraController->OnEvent(event);
	ar::EventDispatcher dispatcher(event);
}

void EditorLayer::OnImGuiRender() 
{
	m_UI.Render();
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
	float xPos = m_State.ClickPosition.x;
	float yPos = m_State.ClickPosition.y;

	// convert to NDC
	float xNDC = (2.0f * xPos) / m_State.Viewport.Width - 1.0f;
	float yNDC = 1.0f - (2.0f * yPos) / m_State.Viewport.Height;
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
	
	// temporary
	m_State.CursorPosition = cursorPos;
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

