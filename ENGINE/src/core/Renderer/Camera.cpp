#include "arpch.h"
#include "Camera.h"
#include <transformations.h>

namespace ar
{
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearPlane,
		float farPlane, float initArcballRadius)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane),
			m_ArcballRadius(initArcballRadius),
			m_Position(0.0f, 0.0f, -5.0f, 1.0f), m_Target(0.0f, 0.0f, 0.0f, 1.0f),
			m_RotationRPY(-15.0f, 15.0f, 0.0f), 
			m_Forward(Vec4::s_WorldForward), m_Up(Vec4::s_WorldUp), m_Right(Vec4::s_WorldRight),
			m_Projection(Identity()), m_InvProjection(Identity()), m_View(Identity()), m_InvView(Identity())
	{
		UpdatePosition();
		m_View = LookAt(m_Right, m_Up, m_Forward, m_Position);
		m_InvView = InvLookAt(m_Right, m_Up, m_Forward, m_Position);
		m_Projection = Perspective(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
		m_InvProjection = InvPerspective(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
	}

	void PerspectiveCamera::UpdateRotation(float dPitch, float dYaw, float dRoll)
	{
		// Change in rotation
		m_RotationRPY.x += dPitch;
		m_RotationRPY.y += dYaw;
		m_RotationRPY.z += dRoll;
		m_RotationQuat = RPYToQuat(m_RotationRPY);

		// Change of the local frame's orientation
		m_Forward = Normalize(Rotate(Vec4::s_WorldForward, m_RotationQuat));
		m_Up = Normalize(Rotate(Vec4::s_WorldUp, m_RotationQuat));
		m_Right = Normalize(Rotate(Vec4::s_WorldRight, m_RotationQuat));

		UpdatePosition();
		m_View = LookAt(m_Right, m_Up, m_Forward, m_Position);
		m_InvView = InvLookAt(m_Right, m_Up, m_Forward, m_Position);
	}

	void PerspectiveCamera::UpdatePosition()
	{
		m_Position = m_Target - m_Forward * m_ArcballRadius;
	}
}

