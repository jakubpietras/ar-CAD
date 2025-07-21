#include "arpch.h"
#include "RenderCommand.h"
#include "platform/OpenGL/OGLRendererAPI.h"

namespace ar
{
	bool RenderCommand::s_DepthTest = true;

	void RenderCommand::SetViewport(int x, int y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::ToggleDepthTest(bool enabled)
	{
		if (enabled) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	ar::RendererAPI* RenderCommand::s_RendererAPI = new ar::OGLRendererAPI();
}