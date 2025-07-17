#include "arpch.h"
#include "Shader.h"
#include "RendererAPI.h"
#include "platform/OpenGL/OGLShader.h"
namespace ar
{
	Shader* Shader::Create(const std::string& vsPath, const std::string& fsPath, const std::string& tcsPath /*= ""*/, const std::string& tesPath /*= ""*/)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return new OGLShader(vsPath, fsPath, tcsPath, tesPath);
		default:
			AR_ASSERT(false, "Uknown renderer");
			return nullptr;
		}
	}
}

