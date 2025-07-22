#pragma once

namespace ar
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera();
		
		inline const mat::Mat4& GetView() { return m_View; }
		inline const mat::Mat4& GetProjection() { return m_Projection; }
		inline const mat::Mat4& GetInvView() { return m_InvView; }
		inline const mat::Mat4& GetInvProjection() { return m_InvProjection; }
		inline const mat::Mat4 GetVP() { return m_Projection * m_View; }
		inline const mat::Vec4& GetForward() const { return m_Forward; }
		inline const mat::Vec4& GetRight() const { return m_Right; }

		void UpdateView(mat::Vec4 position);
		void UpdateProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
		void UpdateOrientation(mat::Quat rotation);
	
	private:	
		mat::Vec4 m_Forward, m_Up, m_Right;

		// Regular projection
		mat::Mat4 m_Projection, m_InvProjection, m_View, m_InvView;
		
		// Stereoscopy
		// TODO: ...
	};
}

