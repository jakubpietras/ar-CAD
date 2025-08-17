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
		s_Shaders["BasicColor"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/default.vert",
			"resources/shaders/OpenGL/default.frag"
		));
		s_Shaders["Points"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/points.vert",
			"resources/shaders/OpenGL/points.frag"
		));
		s_Shaders["Picking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/basic.vert",
			"resources/shaders/OpenGL/picking.frag"
		));
		s_Shaders["CurveC0"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/bezier.vert",
			"resources/shaders/OpenGL/bezier.frag",
			"resources/shaders/OpenGL/bezier.tesc",
			"resources/shaders/OpenGL/bezier.tese"
		));
		s_Shaders["CurveC0Picking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/bezier.vert",
			"resources/shaders/OpenGL/bezierPicking.frag",
			"resources/shaders/OpenGL/bezier.tesc",
			"resources/shaders/OpenGL/bezier.tese"
		));
		s_Shaders["CurveC2"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/bezier.vert",
			"resources/shaders/OpenGL/bezier.frag",
			"resources/shaders/OpenGL/bspline.tesc",
			"resources/shaders/OpenGL/bezier.tese"
		));
		s_Shaders["CurveC2Picking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/bezier.vert",
			"resources/shaders/OpenGL/bezierPicking.frag",
			"resources/shaders/OpenGL/bspline.tesc",
			"resources/shaders/OpenGL/bezier.tese"
		));

	}

	ar::Ref<ar::Shader> ShaderLib::Get(const std::string& name)
	{
		return s_Shaders[name];
	}

}

