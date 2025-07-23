#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

namespace ar
{
	class CameraController
	{
	public:
		enum class CameraEye
		{
			LEFT,
			RIGHT,
			BOTH
		};

		CameraController(float fov, float aspectRatio, float nearPlane,
			float farPlane, float initArcballRadius);
		void OnUpdate();
		void OnEvent(Event& e);
		
		inline std::shared_ptr<PerspectiveCamera> GetCamera() const { return m_Camera; }
		inline const mat::Vec4& GetPosition() { return m_Position; }

		void Rotate(float dPitchDeg, float dYawDeg, float dRollDeg);
		void UpdateArcballRadius(float dRadius);
		void SetAspectRatio(float aspectRatio);

		void UpdatePosition();
		void UpdateRotation(float dPitchDeg, float dYawDeg, float dRollDeg);

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		mat::Vec4 m_Position, m_Target;
		float m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane;
		float m_ArcballRadius;
		mat::Quat m_RotationQuat; mat::Vec3 m_RotationRPY;
		float m_MoveSpeed;


		std::shared_ptr<PerspectiveCamera> m_Camera;
	};
}

