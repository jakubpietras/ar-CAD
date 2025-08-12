#include "arpch.h"
#include "OGLShader.h"

namespace ar
{

	OGLShader::OGLShader(const std::string& vsPath, const std::string& fsPath, const std::string& tcsPath /*= ""*/, const std::string& tesPath /*= ""*/)
	{
		std::vector<GLuint> shaders;
		std::string vertexSource = LoadSource(vsPath),
			fragmentSource = LoadSource(fsPath);
		shaders.push_back(CompileShader(GL_VERTEX_SHADER, vertexSource));
		shaders.push_back(CompileShader(GL_FRAGMENT_SHADER, fragmentSource));
		if (!tcsPath.empty())
			shaders.push_back(CompileShader(GL_TESS_CONTROL_SHADER, LoadSource(tcsPath)));
		
		if (!tesPath.empty())
			shaders.push_back(CompileShader(GL_TESS_EVALUATION_SHADER, LoadSource(tesPath)));
		
		LinkProgram(shaders);
		DeleteShaders(shaders);
	}

	OGLShader::~OGLShader()
	{
		glDeleteProgram(m_programID);
	}

	void OGLShader::Use() const
	{
		glUseProgram(m_programID);
	}

	void OGLShader::SetVec2(const std::string& name, const float value[2]) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform2fv(m_programID, location, 1, value);
	}

	void OGLShader::SetVec3(const std::string& name, const mat::Vec3& value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform3fv(m_programID, location, 1, value.Data());
	}

	void OGLShader::SetMat4(const std::string& name, const mat::Mat4& value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniformMatrix4fv(m_programID, location, 1, GL_FALSE, value.data.data());
	}

	void OGLShader::SetInt(const std::string& name, int value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform1i(m_programID, location, value);
	}

	void OGLShader::SetFloat(const std::string& name, float value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform1f(m_programID, location, value);
	}

	void OGLShader::SetBool(const std::string& name, bool value) const
	{
		if (value)
			SetInt(name, 1);
		else
			SetInt(name, 0);
	}

	void OGLShader::SetUInt(const std::string& name, uint32_t value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform1ui(m_programID, location, value);
	}

	std::string OGLShader::LoadSource(const std::string& path)
	{
		std::ifstream istrm(path);
		AR_ASSERT(istrm.is_open(), "Failed to open {0}", path);

		std::stringstream ss;
		ss << istrm.rdbuf();
		istrm.close();
		return ss.str();
	}

	uint32_t OGLShader::CompileShader(uint32_t shaderType, const std::string& shaderSource)
	{
		const char* source = shaderSource.c_str();
		auto shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &source, nullptr);

		glCompileShader(shader);
		char infoLog[512];
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		AR_ASSERT(GL_FALSE != status, "Shader compilation failed: " + std::string(infoLog));
		return shader;
	}

	void OGLShader::LinkProgram(std::vector<uint32_t>& shaders)
	{
		m_programID = glCreateProgram();
		for (auto& shader : shaders)
			glAttachShader(m_programID, shader);

		glLinkProgram(m_programID);
		GLint status;
		glGetProgramiv(m_programID, GL_LINK_STATUS, &status);
		AR_ASSERT(GL_FALSE != status, "Linking a shader program failed.");
	}

	void OGLShader::DeleteShaders(std::vector<uint32_t>& shaders)
	{
		for (auto& shader : shaders)
		{
			glDetachShader(m_programID, shader);
			glDeleteShader(shader);
		}
	}

}