#include "arpch.h"
#include "RenderCommand.h"
#include "platform/OpenGL/OGLRendererAPI.h"

namespace ar
{
	bool RenderCommand::s_DepthTest = true;
	bool RenderCommand::s_BlendColor = true;

	void RenderCommand::SetViewport(int x, int y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::SetLineThickness(uint32_t width)
	{
		glLineWidth(width);
	}

	void RenderCommand::ToggleDepthTest(bool enabled)
	{
		if (enabled) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	void RenderCommand::ToggleBlendColor(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else glDisable(GL_BLEND);
	}

	void RenderCommand::SetDepthMask(uint32_t flag)
	{
		glDepthMask(flag);
	}

	void RenderCommand::SetTessellationPatchSize(size_t size)
	{
		glPatchParameteri(GL_PATCH_VERTICES, size);
	}

	ar::RendererAPI* RenderCommand::s_RendererAPI = new ar::OGLRendererAPI();
}