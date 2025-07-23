#include "arpch.h"
#include "CameraController.h"
#include "Input.h"

namespace ar
{
	CameraController::CameraController(float fov, float aspectRatio, float nearPlane,
		float farPlane, float initArcballRadius)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane),
		m_ArcballRadius(initArcballRadius), m_Camera(std::make_shared<PerspectiveCamera>()),
		m_MoveSpeed(0.01f)
	{
		Rotate(-30, -25, 0);
		m_Camera->UpdateProjection(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
		UpdatePosition();
		m_Camera->UpdateView(m_Position);
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
		dispatcher.Dispatch<KeyPressedEvent>(AR_BIND_EVENT_FN(CameraController::OnKeyPressed));
	}

	void CameraController::Rotate(float dPitchDeg, float dYawDeg, float dRollDeg)
	{
		UpdateRotation(dPitchDeg, dYawDeg, dRollDeg);
		// Rotation causes the local frame to change its basis vectors:
		m_Camera->UpdateOrientation(m_RotationQuat);
		// Based on the new orientation we calculate the position:
		UpdatePosition();
		// Based on the new position and local frame we calculate view matrix:
		m_Camera->UpdateView(m_Position);
	}

	void CameraController::UpdateArcballRadius(float dRadius)
	{
		m_ArcballRadius += dRadius;
		UpdatePosition();
		// Based on the new position we calculate view matrix:
		m_Camera->UpdateView(m_Position);
	}

	void CameraController::SetAspectRatio(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		m_Camera->UpdateProjection(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
	}

	void CameraController::UpdatePosition()
	{
		m_Position = m_Target - m_Camera->GetForward() * m_ArcballRadius;
	}

	void CameraController::UpdateRotation(float dPitchDeg, float dYawDeg, float dRollDeg)
	{
		m_RotationRPY.x += dPitchDeg;
		m_RotationRPY.y += dYawDeg;
		m_RotationRPY.z += dRollDeg;
		m_RotationQuat = RPYToQuat(m_RotationRPY);
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		UpdateArcballRadius(-e.GetYOffset());
		return false;
	}

	bool CameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (Input::IsMouseButtonPressed(AR_MOUSE_BUTTON_MIDDLE))
		{
			Rotate(e.GetYOffset(), -e.GetXOffset(), 0.0f);
		}
		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		float aspectRatio = static_cast<float>(e.GetWidth()) / e.GetHeight();
		SetAspectRatio(aspectRatio);
		return false;
	}

	bool CameraController::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == AR_KEY_W)
		{
			m_Target.x += (-m_MoveSpeed);
			UpdatePosition();
		}

		return false;
	}
}