#pragma once
#include <glad/glad.h>
#include "core/Renderer/Shader.h"

namespace ar
{
	class OGLShader : public Shader
	{
	public:
		OGLShader(const std::string& vsPath,
			const std::string& fsPath,
			const std::string& tcsPath = "",
			const std::string& tesPath = "");
		~OGLShader();
		void Use() const override;
		void SetVec2(const std::string& name, const float value[2]) const override;
		void SetVec3(const std::string& name, const mat::Vec3& value) const override;
		void SetMat4(const std::string& name, const mat::Mat4& value) const override;
		void SetInt(const std::string& name, int value) const override;
		void SetFloat(const std::string& name, float value) const override;
		void SetBool(const std::string& name, bool value) const override;
		void SetUInt(const std::string& name, uint32_t value) const override;

	protected:
		std::string LoadSource(const std::string& path) override;
		uint32_t CompileShader(uint32_t shaderType, const std::string& shaderSource) override;
		void LinkProgram(std::vector<uint32_t>& shaders) override;
		void DeleteShaders(std::vector<uint32_t>& shaders) override;
	private:
		uint32_t m_programID;
	};
}

