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
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		bool Ignore(MouseScrolledEvent& e);

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}

