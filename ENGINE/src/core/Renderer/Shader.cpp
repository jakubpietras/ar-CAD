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
		s_Shaders["Torus"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/torus.vert",
			"resources/shaders/OpenGL/torus.frag"
		));
		s_Shaders["TorusPicking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/torus.vert",
			"resources/shaders/OpenGL/torusPicking.frag"
		));
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
		s_Shaders["Point"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/point.vert",
			"resources/shaders/OpenGL/point.frag"
		));

		s_Shaders["PointPicking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/point.vert",
			"resources/shaders/OpenGL/picking.frag"
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
		s_Shaders["SurfaceC0"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/surface.vert",
			"resources/shaders/OpenGL/surface.frag",
			"resources/shaders/OpenGL/surface.tesc",
			"resources/shaders/OpenGL/surface.tese"
		));
		s_Shaders["SurfaceC0Picking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/surface.vert",
			"resources/shaders/OpenGL/surfacePicking.frag",
			"resources/shaders/OpenGL/surface.tesc",
			"resources/shaders/OpenGL/surface.tese"
		));
		s_Shaders["SurfaceC2"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/surface.vert",
			"resources/shaders/OpenGL/surface.frag",
			"resources/shaders/OpenGL/surfaceC2.tesc",
			"resources/shaders/OpenGL/surface.tese"
		));
		s_Shaders["SurfaceC2Picking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/surface.vert",
			"resources/shaders/OpenGL/surfacePicking.frag",
			"resources/shaders/OpenGL/surfaceC2.tesc",
			"resources/shaders/OpenGL/surface.tese"
		));
		s_Shaders["Gregory"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/surface.vert",
			"resources/shaders/OpenGL/surface.frag",
			"resources/shaders/OpenGL/gregory.tesc",
			"resources/shaders/OpenGL/gregory.tese"
		));
		s_Shaders["GregoryPicking"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/surface.vert",
			"resources/shaders/OpenGL/surfacePicking.frag",
			"resources/shaders/OpenGL/gregory.tesc",
			"resources/shaders/OpenGL/gregory.tese"
		));
		s_Shaders["Paths"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/paths.vert",
			"resources/shaders/OpenGL/paths.frag"
		));
		s_Shaders["MillingTop"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/millingTop.vert",
			"resources/shaders/OpenGL/millingTop.frag"
		));
		s_Shaders["MillingSide"] = Ref<Shader>(Shader::Create(
			"resources/shaders/OpenGL/millingSide.vert",
			"resources/shaders/OpenGL/millingSide.frag"
		));
	}

	ar::Ref<ar::Shader> ShaderLib::Get(const std::string& name)
	{
		return s_Shaders[name];
	}

}

