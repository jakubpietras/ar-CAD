#include "arpch.h"
#include "Camera.h"
#include <transformations.h>

namespace ar
{
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearPlane,
		float farPlane, float initArcballRadius)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane),
			m_ArcballRadius(initArcballRadius),
			m_Target(0.0f, 0.0f, 0.0f, 1.0f),
			m_Forward(mat::Vec4::s_WorldForward), m_Up(mat::Vec4::s_WorldUp), m_Right(mat::Vec4::s_WorldRight),
			m_Projection(mat::Identity()), m_InvProjection(mat::Identity()), m_View(mat::Identity()), m_InvView(mat::Identity())
	{
		Rotate(-15.0f, 15.0f, 0.0f);
		UpdateProjection();
	}

	void PerspectiveCamera::Rotate(float dPitch, float dYaw, float dRoll)
	{
		UpdateRotation(dPitch, dYaw, dRoll);
		// Rotation causes the local frame to change its basis vectors:
		UpdateOrientation();
		// Based on the new orientation we calculate the position:
		UpdatePosition();
		// Based on the new position and local frame we calculate view matrix:
		UpdateView();
	}
	void PerspectiveCamera::UpdateArcballRadius(float dRadius)
	{
		m_ArcballRadius += dRadius;
		UpdatePosition();
		// Based on the new position we calculate view matrix:
		UpdateView();
	}
	void PerspectiveCamera::UpdatePosition()
	{
		m_Position = m_Target - m_Forward * m_ArcballRadius;
	}
	void PerspectiveCamera::UpdateRotation(float dPitch, float dYaw, float dRoll)
	{
		m_RotationRPY.x += dPitch;
		m_RotationRPY.y += dYaw;
		m_RotationRPY.z += dRoll;
		m_RotationQuat = RPYToQuat(m_RotationRPY);
	}
	void PerspectiveCamera::UpdateView()
	{
		m_View = LookAt(m_Right, m_Up, m_Forward, m_Position);
		m_InvView = InvLookAt(m_Right, m_Up, m_Forward, m_Position);
	}
	void PerspectiveCamera::UpdateProjection()
	{
		m_Projection = mat::Perspective(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
		m_InvProjection = mat::InvPerspective(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
	}
	void PerspectiveCamera::UpdateOrientation()
	{
		m_Forward = mat::Normalize(mat::Rotate(mat::Vec4::s_WorldForward, m_RotationQuat));
		m_Up = mat::Normalize(mat::Rotate(mat::Vec4::s_WorldUp, m_RotationQuat));
		m_Right = mat::Normalize(mat::Rotate(mat::Vec4::s_WorldRight, m_RotationQuat));
	}
}

