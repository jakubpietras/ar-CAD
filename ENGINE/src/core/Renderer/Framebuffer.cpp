#include "arpch.h"
#include "Framebuffer.h"
#include "platform/OpenGL/OGLFramebuffer.h"
#include "RendererAPI.h"

namespace ar
{
	Framebuffer* ar::Framebuffer::Create(const FramebufferDesc& desc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
		{
			return nullptr;
		}
		case RendererAPI::API::OpenGL:
		{
			return new OGLFramebuffer(desc);
		}
		default:
			return nullptr;
		}
	}
}