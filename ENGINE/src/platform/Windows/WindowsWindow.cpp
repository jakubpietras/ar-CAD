#include "arpch.h"
#include "WindowsWindow.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")

#include "core/Input.h"
#include "core/Events/ApplicationEvent.h"
#include "core/Events/MouseEvent.h"
#include "core/Events/KeyEvent.h"

#include "core/Renderer/DeviceContext.h"
#include "platform/OpenGL/OpenGLContext.h"

namespace ar
{
	static bool s_GLFWInitialized = false;
	static bool s_IsDragging = false;
	static float s_LastX = 0.0f, s_LastY = 0.0f;

	static void GLFWErrorCallback(int error, const char* desc)
	{
		AR_CORE_ERROR("GLFW Error {0}: {1}", error, desc);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			AR_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(),
			nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, &m_Data);

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		SetVSync(true);
		
		// Dark title bar
		HWND hwnd = glfwGetWin32Window(m_Window);
		BOOL useDark = TRUE;
		::DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));

		// Icon
		int width, height, channels;
		auto pixels = stbi_load("resources/icons/icon.png", &width, &height, &channels, 0);
		GLFWimage images[1];
		images[0].width = 16;
		images[0].height = 16;
		images[0].pixels = pixels;
		glfwSetWindowIcon(m_Window, 1, images);
		stbi_image_free(pixels);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
				
				WindowResizeEvent event(width, height);
				data.EventCallback(event); // dispatch
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event); // dispatch
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, false);
						data.EventCallback(event); // dispatch
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event); // dispatch
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, true);
						data.EventCallback(event); // dispatch
						break;
					}
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						if (button == AR_MOUSE_BUTTON_MIDDLE)
						{
							double xPos, yPos;
							s_IsDragging = true;
							glfwGetCursorPos(window, &xPos, &yPos);
							s_LastX = static_cast<float>(xPos);
							s_LastY = static_cast<float>(yPos);
						}
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						if (button == AR_MOUSE_BUTTON_MIDDLE)
							s_IsDragging = false;
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				auto x = static_cast<float>(xPos);
				auto y = static_cast<float>(yPos);
				
				if (!s_IsDragging)
					return;
				
				float offsetX = x - s_LastX;
				float offsetY = s_LastY - y;
				s_LastX = x;
				s_LastY = y;

				MouseMovedEvent event(x, y, offsetX, offsetY);
				data.EventCallback(event);
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});


	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}