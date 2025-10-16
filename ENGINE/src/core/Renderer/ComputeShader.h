#pragma once

namespace ar
{
	class ComputeShader
	{
	public:
		virtual ~ComputeShader() {}

		virtual void Use() const = 0;
		static ComputeShader* Create(const std::string& path);
	private:

	protected:
		virtual std::string LoadSource(const std::string& path) = 0;
		virtual uint32_t CompileShader(uint32_t shaderType, const std::string& shaderSource) = 0;
		virtual void LinkProgram(uint32_t shader) = 0;
		virtual void DeleteShader(uint32_t shader) = 0;
	};
}