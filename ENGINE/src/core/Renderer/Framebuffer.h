#pragma once
#include "Texture.h"

namespace ar
{
	struct FramebufferDesc
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;
		TextureFormat Format = TextureFormat::RGBA8;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() {}
		static Framebuffer* Create(const FramebufferDesc& desc);

		virtual void Resize(uint32_t newWidth, uint32_t newHeight) = 0;
		virtual void Invalidate() = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetColorAttachment() = 0;
		virtual uint32_t GetHeight() = 0;
		virtual uint32_t GetWidth() = 0;

	protected:
		uint32_t m_ID;
	};
}

