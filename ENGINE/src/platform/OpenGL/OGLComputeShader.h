#pragma once
#include <glad/glad.h>
#include "core/Renderer/ComputeShader.h"
#include <string>
#include <vector>

namespace ar
{
	class OGLComputeShader : public ComputeShader
	{
	public:
		OGLComputeShader(const std::string& path);
		~OGLComputeShader();
		void Use() const override;
		
	protected:
		std::string LoadSource(const std::string& path) override;
		uint32_t CompileShader(uint32_t shaderType, const std::string& shaderSource) override;
		void LinkProgram(uint32_t shader) override;
		void DeleteShader(uint32_t shader) override;
	private:
		uint32_t m_programID;
	};
}