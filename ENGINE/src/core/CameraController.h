#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

namespace ar
{
	class CameraController
	{
	public:
		CameraController(float fov, float aspectRatio, float nearPlane,
			float farPlane, float initArcballRadius);
		void OnUpdate();
		void OnEvent(Event& e);
		inline std::shared_ptr<PerspectiveCamera> GetCamera() const { return m_Camera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		float m_AspectRatio;
		std::shared_ptr<PerspectiveCamera> m_Camera;
	};
}

