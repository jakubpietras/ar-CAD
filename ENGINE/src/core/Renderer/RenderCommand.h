#pragma once
#include "RendererAPI.h"

namespace ar
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const mat::Vec4 color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }
		static void SetViewport(int x, int y, uint32_t width, uint32_t height);
		static void ToggleDepthTest(bool enabled);
		static void ToggleBlendColor(bool enabled);
		static void SetDepthMask(uint32_t flag);

		inline static void Draw(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->Draw(vertexArray);
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
		inline static void DrawEmpty(uint32_t vertexCount)
		{
			s_RendererAPI->DrawEmpty(vertexCount);
		}
	private:
		static RendererAPI* s_RendererAPI;
		static bool s_DepthTest;
		static bool s_BlendColor;
	};
}

