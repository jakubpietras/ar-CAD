#pragma once
#include "core/Renderer/RendererAPI.h"

namespace ar
{
	class OGLRendererAPI : public RendererAPI
	{
	public:
		void Clear() override;
		void SetClearColor(mat::Vec4 color) override;
		void Draw(const std::shared_ptr<VertexArray>& vertexArray) override;
		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
		void DrawEmpty(uint32_t vertexCount) override;
	};
}

