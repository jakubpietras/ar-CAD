#include "EditorLayer.h"
#include "core/ImGui/ScopedDisable.h"
#include "EditorConstants.h"

EditorLayer::EditorLayer(float aspectRatio)
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
}

void EditorLayer::OnDetach() { }

void EditorLayer::OnUpdate()
{
	m_SceneController.OnUpdateCamera();
	m_SceneController.ProcessStateChanges(m_State);
	m_Scene->OnUpdate(m_SceneController.GetCamera(), m_State.CursorPosition, {0.f, 0.f, 0.f}); // todo: add mean position
	m_SceneRenderer.RenderMain(m_SceneController.GetCameraController(), m_State.CursorPosition);
	m_SceneRenderer.RenderPicking(m_SceneController.GetCameraController());
}

void EditorLayer::OnEvent(ar::Event& event)
{
	m_SceneController.OnEventCamera(event);
	ar::EventDispatcher dispatcher(event);
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
	return false;
}

bool EditorLayer::OnMouseButtonReleased(ar::MouseButtonReleasedEvent& event)
{
	if (event.GetMouseButton() == AR_MOUSE_BUTTON_RIGHT && ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
	{
		m_State.ShouldPlaceCursor = true;
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
