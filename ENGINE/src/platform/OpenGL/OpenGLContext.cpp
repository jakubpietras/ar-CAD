#include "arpch.h"
#include "OpenGLContext.h"
#include <glad/glad.h>

namespace ar
{
	OpenGLContext::OpenGLContext(GLFWwindow* wHandle)
		: m_WindowHandle(wHandle)
	{
		AR_ASSERT(wHandle, "Handle is null")
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress));
		AR_ASSERT(status, "Failed to initialize glad!");

		AR_CORE_INFO("Rendering info:");
		AR_CORE_INFO("	Vendor:  {0}", (const char*)glGetString(GL_VENDOR));
		AR_CORE_INFO("	GPU:     {0}", (const char*)glGetString(GL_RENDERER));
		AR_CORE_INFO("	Version: {0}", (const char*)glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}