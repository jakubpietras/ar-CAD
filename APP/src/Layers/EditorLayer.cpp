#include "EditorLayer.h"
#include "core/ImGui/ScopedDisable.h"
#include "Tools/EditorConstants.h"

EditorLayer::EditorLayer(float aspectRatio)
	: 
	m_Scene(std::make_shared<ar::Scene>()),
	m_UI(m_State, m_Scene),
	m_SceneController(m_Scene)
{
	m_State.Viewport = { 1920.f, 1080.f };
	m_State.ViewportResized = true;
}

void EditorLayer::OnAttach() 
{
	ar::ShaderLib::Init();
}

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_SceneController.OnUpdateCamera();
	m_SceneController.ProcessStateChanges(m_State);
	m_UI.GetFramebuffer()->Bind();

	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
	ar::RenderCommand::Clear();

	m_Scene->OnUpdate(m_SceneController.GetCamera());
	m_UI.RenderCursor(m_SceneController.GetCameraController(), m_State.CursorPosition);

	m_UI.GetFramebuffer()->Unbind();
}

void EditorLayer::OnEvent(ar::Event& event)
{
	m_SceneController.OnEventCamera(event);
	ar::EventDispatcher dispatcher(event);
}

void EditorLayer::OnImGuiRender() 
{
	m_UI.Render();
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
	auto rayCamera = m_SceneController.GetCamera()->GetInvProjection() * rayClip;
	rayCamera = ar::mat::Vec4(rayCamera.x, rayCamera.y, -1.0f, 0.0f);

	// convert to world space
	auto direction = ar::mat::Normalize(ar::mat::ToVec3(m_SceneController.GetCamera()->GetInvView() * rayCamera));
	
	ar::mat::Vec3 origin = ar::mat::ToVec3(m_SceneController.GetCameraController()->GetPosition());
	ar::mat::Vec3 planeNormal = ar::mat::ToVec3(m_SceneController.GetCamera()->GetForward());
	float t = -((ar::mat::Dot(origin, planeNormal)) / ar::mat::Dot(direction, planeNormal));
	auto cursorPos = origin + direction * t;
	
	m_State.CursorPosition = cursorPos;
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

