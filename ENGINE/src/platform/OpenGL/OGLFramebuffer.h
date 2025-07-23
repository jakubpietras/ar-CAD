#pragma once
#include "core/Renderer/Framebuffer.h"
#include "core/Renderer/Texture.h"

namespace ar
{
	class OGLFramebuffer : public Framebuffer
	{
	public:
		OGLFramebuffer(const FramebufferDesc& desc);
		~OGLFramebuffer() override;
		void Invalidate();
		void Resize(uint32_t newWidth, uint32_t newHeight) override;
	private:
		std::unique_ptr<Texture> m_ColorAttachment, m_DepthAttachment;
		FramebufferDesc m_Description;
	};
}

