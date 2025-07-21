#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
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

		void Rotate(float dPitch, float dYaw, float dRoll);
		void UpdateArcballRadius(float dRadius);
		inline void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }

		void UpdatePosition();
		void UpdateRotation(float dPitch, float dYaw, float dRoll);

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		mat::Vec4 m_Position, m_Target;
		float m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane;
		float m_ArcballRadius;
		mat::Quat m_RotationQuat; mat::Vec3 m_RotationRPY;


		std::shared_ptr<PerspectiveCamera> m_Camera;
	};
}

