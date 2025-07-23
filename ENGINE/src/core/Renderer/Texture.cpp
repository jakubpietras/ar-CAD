#include "arpch.h"
#include "Texture.h"
#include "platform/OpenGL/OGLTexture.h"
#include "RendererAPI.h"

namespace ar
{
	
	Texture* Texture::Create(const TextureDesc& desc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
		{
			return nullptr;
		}
		case RendererAPI::API::OpenGL:
		{
			return new OGLTexture(desc);
		}
		default:
			return nullptr;
		}
	}

	ar::Texture* Texture::Create(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
		{
			return nullptr;
		}
		case RendererAPI::API::OpenGL:
		{
			return new OGLTexture(filepath);
		}
		default:
			return nullptr;
		}
	}

}