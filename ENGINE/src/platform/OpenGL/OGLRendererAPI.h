#pragma once
#include "core/Renderer/RendererAPI.h"
#include "core/Renderer/Primitive.h"

namespace ar
{
	class OGLRendererAPI : public RendererAPI
	{
	public:
		void Clear() override;
		void SetClearColor(mat::Vec4 color) override;
		
		void Draw(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount = 1) override;
		
		void DrawIndexed(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount) override;
		
		void DrawEmpty(const Primitive primitive, uint32_t vertexCount,
			uint32_t instanceCount = 1) override;
	private:
		GLenum GetOGLPrimitive(Primitive primitive);
	};
}

