#pragma once

#include "core/Renderer/DeviceContext.h"

struct GLFWwindow;

namespace ar
{
	class OpenGLContext : public DeviceContext
	{
	public:
		OpenGLContext(GLFWwindow* wHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}
