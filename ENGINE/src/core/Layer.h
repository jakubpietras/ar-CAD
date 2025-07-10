#pragma once
#include <string>
#include "Events/Event.h"

namespace ar
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		// when layer is pushed onto the stack
		virtual void OnAttach() {}
		// when layer is removed from the stack
		virtual void OnDetach() {}
		// when layer is updated (every frame)
		virtual void OnUpdate() {}
		// handling events that arrive
		virtual void OnEvent(Event& event) {}

		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}