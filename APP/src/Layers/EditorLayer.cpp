#include "EditorLayer.h"
#include "core/ImGui/ScopedDisable.h"
#include "EditorConstants.h"

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

	m_Scene->OnUpdate(m_SceneController.GetCamera(), m_State.CursorPosition, {0.f, 0.f, 0.f}); // todo: add mean position
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