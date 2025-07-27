#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "Primitive.h"
#include "core/Scene/Components.h"

namespace ar
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Submit(const Primitive primitive, const std::shared_ptr<Shader>& shader, 
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount = 1);
		static void Submit(const Primitive primitive, const std::shared_ptr<Shader>& shader,
			uint32_t vertexCount, uint32_t instanceCount = 1);
		static void Submit(MeshComponent& mesh, uint32_t instanceCount = 1);
	private:
		static std::unique_ptr<VertexArray> s_DummyVAO;
	};
}