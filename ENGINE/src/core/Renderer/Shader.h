#pragma once

namespace ar
{
	class Shader
	{
	public:
		virtual ~Shader() {}
		
		virtual void Use() const = 0;
		virtual void SetVec2(const std::string& name, const float value[2]) const = 0;
		virtual void SetVec3(const std::string& name, const mat::Vec3& value) const = 0;
		virtual void SetMat4(const std::string& name, const mat::Mat4& value) const = 0;
		virtual void SetInt(const std::string& name, int value) const = 0;
		virtual void SetFloat(const std::string& name, float value) const = 0;
		virtual void SetBool(const std::string& name, bool value) const = 0;

		static Shader* Create(const std::string& vsPath,
			const std::string& fsPath,
			const std::string& tcsPath = "",
			const std::string& tesPath = "");

	protected:
		virtual std::string LoadSource(const std::string& path) = 0;
		virtual uint32_t CompileShader(uint32_t shaderType, const std::string& shaderSource) = 0;
		virtual void LinkProgram(std::vector<uint32_t>& shaders) = 0;
		virtual void DeleteShaders(std::vector<uint32_t>& shaders) = 0;
	};
}

