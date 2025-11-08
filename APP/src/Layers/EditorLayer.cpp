#include "EditorLayer.h"
#include "core/ImGui/ScopedDisable.h"
#include "EditorConstants.h"
#include "core/Scene/DebugRenderer.h"

EditorLayer::EditorLayer()
	: 
	m_Scene(std::make_shared<ar::Scene>()),
	m_UI(m_State, m_Scene),
	m_SceneController(m_Scene, m_SceneRenderer),
	m_SceneRenderer(m_Scene)
{
	m_State.Viewport = { 1920.f, 1080.f };
	m_State.ViewportResized = true;
}

void EditorLayer::OnAttach() 
{
	ar::ShaderLib::Init();
	ar::DebugRenderer::Init();
}

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_SceneController.OnUpdateCamera();
	m_SceneController.ProcessStateChanges(m_State);
	m_Scene->OnUpdate(m_SceneController.GetCamera(), m_State.CursorPosition, m_State.SelectedMeanPosition);

	ar::mat::Vec2 viewport = { m_State.Viewport.Width, m_State.Viewport.Height };
	m_SceneRenderer.RenderMain(m_SceneController.GetCameraController(), viewport, m_State.CursorPosition, m_State.SelectedMeanPosition, 
		m_State.SelectedObjectsWithTransforms.size() > 1);
	m_SceneRenderer.RenderPicking(m_SceneController.GetCameraController(), viewport);
	
}

void EditorLayer::OnEvent(ar::Event& event)
{
	m_SceneController.OnEventCamera(event);
	ar::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<ar::MouseMovedEvent>(AR_BIND_EVENT_FN(EditorLayer::OnMouseMoved));
	dispatcher.Dispatch<ar::MouseButtonPressedEvent>(AR_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	dispatcher.Dispatch<ar::MouseButtonReleasedEvent>(AR_BIND_EVENT_FN(EditorLayer::OnMouseButtonReleased));
}

void EditorLayer::OnImGuiRender() 
{
	m_UI.Render(m_SceneRenderer.GetMainFramebuffer());
}

bool EditorLayer::OnMouseButtonPressed(ar::MouseButtonPressedEvent& event)
{
	if (event.GetMouseButton() == AR_MOUSE_BUTTON_LEFT)
	{
		m_State.PickClickStart = m_State.MousePosViewport;
		m_State.BoxStart = m_State.MousePosGlobal;
		m_State.IsBoxPicking = true;
	}
	if (event.GetMouseButton() == AR_MOUSE_BUTTON_RIGHT)
	{
		m_State.EntityGrabbed = true;
		if (ar::Input::IsKeyPressed(AR_KEY_LEFT_CONTROL))
			m_State.MoveVertical = true;
		else if (ar::Input::IsKeyPressed(AR_KEY_LEFT_ALT))
			m_State.MoveVertical = false;
		else
			m_State.EntityGrabbed = false;
	}
	return false;
}

bool EditorLayer::OnMouseButtonReleased(ar::MouseButtonReleasedEvent& event)
{
	if (event.GetMouseButton() == AR_MOUSE_BUTTON_RIGHT)
	{
		if (ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
			m_State.ShouldPlaceCursor = true;
		m_State.EntityGrabbed = false;
		m_State.MoveVertical = false;
	}
	else if (event.GetMouseButton() == AR_MOUSE_BUTTON_LEFT)
	{
		m_State.PickClickEnd = m_State.MousePosViewport;
		if (ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
			m_State.PickingMode = SelectionMode::Add;
		else
			m_State.PickingMode = SelectionMode::Replace;
		m_State.ShouldProcessPicking = true;
		m_State.IsBoxPicking = false;
	}
	return false;
}

bool EditorLayer::OnMouseMoved(ar::MouseMovedEvent& event)
{
	m_State.MoveDx = event.GetXOffset();
	m_State.MoveDy = event.GetYOffset();
	return false;
}
