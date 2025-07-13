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
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}