#pragma once
#include "RendererAPI.h"

namespace ar
{
	class RenderCommand
	{
	public:

		inline static void SetClearColor(const mat::Vec4 color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void Draw(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->Draw(vertexArray);
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}

