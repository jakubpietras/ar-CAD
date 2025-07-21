#include "arpch.h"
#include "RenderCommand.h"
#include "platform/OpenGL/OGLRendererAPI.h"

namespace ar
{

	void RenderCommand::SetViewport(int x, int y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	ar::RendererAPI* RenderCommand::s_RendererAPI = new ar::OGLRendererAPI();
}