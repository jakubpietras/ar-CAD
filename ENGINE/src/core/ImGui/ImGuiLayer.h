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

		void Begin();
		void End();

		void BlockEvents(bool status) override;

	private:
		void SetBessTheme();
		float m_Time = 0.0f;
	};
}

