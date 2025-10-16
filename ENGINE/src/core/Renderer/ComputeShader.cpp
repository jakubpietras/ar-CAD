#include "arpch.h"
#include "ComputeShader.h"
#include "RendererAPI.h"
#include "platform/OpenGL/OGLComputeShader.h"

namespace ar
{
	ComputeShader* ComputeShader::Create(const std::string& path)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return new OGLComputeShader(path);
		default:
			AR_ASSERT(false, "Uknown renderer");
			return nullptr;
		}
	}
} 