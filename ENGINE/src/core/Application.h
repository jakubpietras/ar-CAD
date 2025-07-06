#pragma once
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"


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
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};
}

