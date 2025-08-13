#include "arpch.h"
#include "WindowsInput.h"

#include <GLFW/glfw3.h>

namespace ar
{
	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	bool WindowsInput::IsAnyMouseButtonPressedImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		static const std::vector<uint32_t> codes = { AR_MOUSE_BUTTON_LEFT, AR_MOUSE_BUTTON_RIGHT, AR_MOUSE_BUTTON_MIDDLE };
		for (const auto& code : codes)
		{
			if (glfwGetMouseButton(window, code) == GLFW_PRESS)
				return true;
		}
		return false;
	}
	float WindowsInput::GetMouseXImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		
		return (float)xpos;
	}
	float WindowsInput::GetMouseYImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return (float)ypos;
	}

	bool WindowsInput::IsAnyMouseButtonReleasedImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		static const std::vector<uint32_t> codes = { AR_MOUSE_BUTTON_LEFT, AR_MOUSE_BUTTON_RIGHT, AR_MOUSE_BUTTON_MIDDLE };
		for (const auto& code : codes)
		{
			if (glfwGetMouseButton(window, code) == GLFW_RELEASE)
				return true;
		}
		return false;
	}

}
