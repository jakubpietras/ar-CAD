#pragma once
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Layer.h"
#include "LayerStack.h"

namespace ar
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
	
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};
}

