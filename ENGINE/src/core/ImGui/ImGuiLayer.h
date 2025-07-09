#pragma once
#include "core/Layer.h"
#include "core/Events/MouseEvent.h"
#include "core/Events/KeyEvent.h"
#include "core/Events/ApplicationEvent.h"
#include "imgui.h"

namespace ar
{
	class ImGuiLayer : public ar::Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);

		ImGuiKey KeyToImGuiKey(int keycode);

		float m_Time = 0.0f;
	};
}

