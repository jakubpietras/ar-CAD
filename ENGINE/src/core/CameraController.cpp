#include "arpch.h"
#include "CameraController.h"
#include "Input.h"

namespace ar
{
	CameraController::CameraController(float fov, float aspectRatio, float nearPlane,
		float farPlane, float initArcballRadius)
		: m_Camera(std::make_shared<PerspectiveCamera>(fov, aspectRatio, nearPlane, farPlane, initArcballRadius)),
		m_AspectRatio(aspectRatio)
	{
	}

	void CameraController::OnUpdate()
	{
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseMovedEvent>(AR_BIND_EVENT_FN(CameraController::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(AR_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(AR_BIND_EVENT_FN(CameraController::OnWindowResized));
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_Camera->UpdateArcballRadius(-e.GetYOffset());
		return false;
	}

	bool CameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (Input::IsMouseButtonPressed(AR_MOUSE_BUTTON_MIDDLE))
		{
			m_Camera->Rotate(e.GetYOffset(), -e.GetXOffset(), 0.0f);
		}
		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		float aspectRatio = static_cast<float>(e.GetWidth()) / e.GetHeight();
		m_Camera->SetAspectRatio(aspectRatio);
		return false;
	}

}