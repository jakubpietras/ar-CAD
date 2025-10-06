#include "arpch.h"
#include "Camera.h"
#include <transformations.h>
#include "Constants.h"

namespace ar
{
	PerspectiveCamera::PerspectiveCamera()
		: m_Forward(ar::mat::WorldForward), m_Up(ar::mat::WorldUp), m_Right(ar::mat::WorldRight),
			m_Projection(mat::Identity()), m_InvProjection(mat::Identity()), 
			m_View(mat::Identity()), m_InvView(mat::Identity())
	{
		m_ViewProjection = m_Projection * m_View;
	}

	void PerspectiveCamera::UpdateView(mat::Vec4 position, mat::Vec3 offset)
	{
		m_View = LookAt(m_Right, m_Up, m_Forward, position) * mat::TranslationMatrix(offset);
		m_ViewProjection = m_Projection * m_View;
		m_InvView = mat::TranslationMatrix(-offset) * InvLookAt(m_Right, m_Up, m_Forward, position);
	}
	void PerspectiveCamera::UpdateProjection(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		m_Projection = mat::Perspective(fov, aspectRatio, nearPlane, farPlane);
		m_ViewProjection = m_Projection * m_View;
		m_InvProjection = mat::InvPerspective(fov, aspectRatio, nearPlane, farPlane);
	}
	void PerspectiveCamera::UpdateOrientation(mat::Quat rotation)
	{
		m_Forward = mat::Normalize(mat::Rotate(ar::mat::WorldForward, rotation));
		m_Up = mat::Normalize(mat::Rotate(ar::mat::WorldUp, rotation));
		m_Right = mat::Normalize(mat::Rotate(ar::mat::WorldRight, rotation));
	}
}

