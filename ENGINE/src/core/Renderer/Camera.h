#pragma once

namespace ar
{
	class PerspectiveCamera
	{
	public:
		enum class CameraEye
		{
			LEFT,
			RIGHT,
			BOTH
		};

		PerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane, float initArcballRadius);
		
		inline const mat::Mat4& GetView() { return m_View; }
		inline const mat::Mat4& GetProjection() { return m_Projection; }
		inline const mat::Mat4& GetInvView() { return m_InvView; }
		inline const mat::Mat4& GetInvProjection() { return m_InvProjection; }
		inline const mat::Mat4& GetVP() { return m_Projection * m_View; }
		inline const mat::Vec4& GetPosition() { return m_Position; }
		
		inline void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }

		void Rotate(float dPitch, float dYaw, float dRoll);
		void UpdateArcballRadius(float dRadius);

	private:
		// Helpers
		void UpdatePosition();
		void UpdateRotation(float dPitch, float dYaw, float dRoll);
		void UpdateView();
		void UpdateProjection();
		void UpdateOrientation();
		
		// Camera properties
		float m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane;
		float m_ArcballRadius;
		mat::Vec4 m_Position, m_Target;
		mat::Quat m_RotationQuat; mat::Vec3 m_RotationRPY;
		mat::Vec4 m_Forward, m_Up, m_Right;

		// Regular projection
		mat::Mat4 m_Projection, m_InvProjection, m_View, m_InvView;
		
		// Stereoscopy
		// CameraEye m_CameraEye;
		// float m_EyeDistance, m_Convergence;
		// TODO: ...
	};
}

