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

	std::unordered_map<std::string, Ref<Shader>> ShaderLib::s_Shaders{};

	void ShaderLib::Init()
	{
		s_Shaders["Basic"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/basic.vert",
			"resources/shaders/OpenGL/basic.frag"));
		s_Shaders["Grid"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/grid.vert",
			"resources/shaders/OpenGL/grid.frag"
		));
	}

	ar::Ref<ar::Shader> ShaderLib::Get(const std::string& name)
	{
		return s_Shaders[name];
	}

}

