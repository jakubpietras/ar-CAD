#pragma once

#include "Event.h"

namespace ar
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y, float dx, float dy)
			: m_MouseX(x), m_MouseY(y), m_OffsetX(dx), m_OffsetY(dy) { }

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }
		inline float GetXOffset() const { return m_OffsetX; }
		inline float GetYOffset() const { return m_OffsetY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY << " [change: " << m_OffsetX << ", " << m_OffsetY << "]";
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_MouseX, m_MouseY, m_OffsetX, m_OffsetY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseScrolled)
	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }
	protected:
		MouseButtonEvent(int button)
			: m_Button(button) { }
		int m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonPressed)

	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonReleased)

	};

}