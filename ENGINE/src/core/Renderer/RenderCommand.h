#pragma once
#include "RendererAPI.h"
#include "Primitive.h"

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

		inline static void Draw(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount = 1)
		{
			s_RendererAPI->Draw(primitive, vertexArray, instanceCount);
		}
		inline static void DrawIndexed(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount)
		{
			s_RendererAPI->DrawIndexed(primitive, vertexArray, instanceCount);
		}
		inline static void DrawEmpty(const Primitive primitive, uint32_t vertexCount, 
			uint32_t instanceCount = 1)
		{
			s_RendererAPI->DrawEmpty(primitive, vertexCount, instanceCount);
		}

	private:
		static RendererAPI* s_RendererAPI;
		static bool s_DepthTest;
		static bool s_BlendColor;
	};
}

