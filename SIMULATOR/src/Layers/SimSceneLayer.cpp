#include "SimSceneLayer.h"
#include "SimEditorConstants.h"


SimSceneLayer::SimSceneLayer(SimState& state)
	: m_State(state),
	m_Renderer(std::make_shared<SimRenderer>()),
	m_UI(std::make_shared<SimUIController>(state)),
	m_Camera(std::make_shared<ar::CameraController>(
		SimEditorCameraConstants::FOV, 0.5f,
		SimEditorCameraConstants::NearPlane, SimEditorCameraConstants::FarPlane,
		SimEditorCameraConstants::ArcballRadius))
{
	m_State.Viewport = { 1920.f, 1080.f };
	m_State.ViewportResized = true;
}

void SimSceneLayer::OnAttach()
{
	ar::ShaderLib::Init();
}

void SimSceneLayer::OnDetach() { }

void SimSceneLayer::OnUpdate()
{
	m_Camera->OnUpdate();
	ProcessStateChanges();
	ar::mat::Vec2 viewport = { m_State.Viewport.Width, m_State.Viewport.Height };
	m_Renderer->Render(m_Camera, viewport);
}

void SimSceneLayer::OnEvent(ar::Event& event)
{
	m_Camera->OnEvent(event);
}

void SimSceneLayer::OnImGuiRender()
{
	m_UI->Render(m_Renderer->GetFramebuffer());
}

void SimSceneLayer::ProcessStateChanges()
{
	if (m_State.ViewportResized)
	{
		m_Camera->SetAspectRatio(m_State.Viewport.Width / m_State.Viewport.Height);
		m_Renderer->OnResize({ m_State.Viewport.Width, m_State.Viewport.Height });
		m_State.ViewportResized = false;
	}
}

