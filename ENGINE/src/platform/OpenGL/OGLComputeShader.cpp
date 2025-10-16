#include "arpch.h"
#include "OGLComputeShader.h"

namespace ar
{
	OGLComputeShader::OGLComputeShader(const std::string& path)
	{
		std::string src = LoadSource(path);
		auto shader = CompileShader(GL_COMPUTE_SHADER, src);
		LinkProgram(shader);
		DeleteShader(shader);
	}
	OGLComputeShader::~OGLComputeShader()
	{
		glDeleteProgram(m_programID);
	}
	void OGLComputeShader::Use() const
	{
		glUseProgram(m_programID);
	}
	std::string OGLComputeShader::LoadSource(const std::string& path)
	{
		std::ifstream istrm(path);
		AR_ASSERT(istrm.is_open(), "Failed to open {0}", path);

		std::stringstream ss;
		ss << istrm.rdbuf();
		istrm.close();
		return ss.str();
	}
	uint32_t OGLComputeShader::CompileShader(uint32_t shaderType, const std::string& shaderSource)
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
	void OGLComputeShader::LinkProgram(uint32_t shader)
	{
		m_programID = glCreateProgram();
		glAttachShader(m_programID, shader);
		glLinkProgram(m_programID);

		GLint status;
		glGetProgramiv(m_programID, GL_LINK_STATUS, &status);

		if (status == GL_FALSE) 
		{
			GLint logLength;
			glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &logLength);

			if (logLength > 0) 
			{
				std::vector<char> log(logLength);
				glGetProgramInfoLog(m_programID, logLength, nullptr, log.data());

				glDeleteProgram(m_programID);
				m_programID = 0;

				std::string errorMsg = "Shader program linking failed:\n";
				errorMsg += std::string(log.begin(), log.end());

				AR_ASSERT(false, errorMsg.c_str());
			}
			else 
			{
				glDeleteProgram(m_programID);
				m_programID = 0;
				AR_ASSERT(false, "Shader program linking failed with no error log.");
			}
		}
	}
	void OGLComputeShader::DeleteShader(uint32_t shader)
	{
		glDetachShader(m_programID, shader);
		glDeleteShader(shader);
	}
	void OGLComputeShader::SetFloat(const std::string& name, float value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform1f(m_programID, location, value);
	}
	void OGLComputeShader::SetBool(const std::string& name, bool value) const
	{
		if (value)
			SetInt(name, 1);
		else
			SetInt(name, 0);
	}
	void OGLComputeShader::SetUInt(const std::string& name, uint32_t value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform1ui(m_programID, location, value);
	}
	void OGLComputeShader::SetInt(const std::string& name, int value) const
	{
		auto location = glGetUniformLocation(m_programID, name.c_str());
		glProgramUniform1i(m_programID, location, value);
	}
}