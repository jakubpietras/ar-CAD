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
		
		inline const Mat4& GetView() { return m_View; }
		inline const Mat4& GetProjection() { return m_Projection; }
		inline const Mat4& GetInvView() { return m_InvView; }
		inline const Mat4& GetInvProjection() { return m_InvProjection; }

		inline const Vec4& GetPosition() { return m_Position; }
		
		void UpdateRotation(float dPitch, float dYaw, float dRoll);
		void UpdatePosition();



	private:
		// Camera properties
		float m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane;
		float m_ArcballRadius;
		Vec4 m_Position, m_Target;
		Quat m_RotationQuat; Vec3 m_RotationRPY;
		Vec4 m_Forward, m_Up, m_Right;

		// Regular projection
		Mat4 m_Projection, m_InvProjection, m_View, m_InvView;
		
		// Stereoscopy
		// CameraEye m_CameraEye;
		// float m_EyeDistance, m_Convergence;
		// TODO: ...
	};
}

