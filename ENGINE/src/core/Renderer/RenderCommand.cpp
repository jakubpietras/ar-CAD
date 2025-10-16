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

	void RenderCommand::SetLineThickness(float width)
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
		glPatchParameteri(GL_PATCH_VERTICES, static_cast<GLint>(size));
	}

	void RenderCommand::BindTexture(ar::Ref<ar::Texture> texture, size_t slot)
	{
		texture->BindSlot(slot);
	}

	void RenderCommand::SetPointSize(float size)
	{
		glPointSize(size);
	}

	inline void RenderCommand::DispatchCompute(Ref<ComputeShader> shader, uint32_t x, uint32_t y, uint32_t z)
	{
		shader->Use();
		glDispatchCompute(x, y, z);
		glUseProgram(0);
	}

	ar::RendererAPI* RenderCommand::s_RendererAPI = new ar::OGLRendererAPI();
}