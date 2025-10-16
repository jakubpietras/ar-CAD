#pragma once
#include "RendererAPI.h"
#include "Primitive.h"
#include "LogUtils.h"
#include "ComputeShader.h"

namespace ar
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const mat::Vec4 color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }
		static void SetViewport(int x, int y, uint32_t width, uint32_t height);
		static void SetLineThickness(float width = 1.);
		static void ToggleDepthTest(bool enabled);
		static void ToggleBlendColor(bool enabled);
		static void SetDepthMask(uint32_t flag);
		static void SetTessellationPatchSize(size_t size);
		static void BindTexture(ar::Ref<ar::Texture> texture, size_t slot);
		static void SetPointSize(float size = 1.);

		inline static void Draw(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount = 1)
		{
			AR_ASSERT(vertexArray->GetVertexCount(), "Drawing from empty buffers");
			s_RendererAPI->Draw(primitive, vertexArray, instanceCount);
		}
		inline static void DrawIndexed(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount)
		{
			AR_ASSERT(vertexArray->GetVertexCount(), "Drawing from empty vertex buffers");
			AR_ASSERT(vertexArray->GetIndexCount(), "Drawing with empty index buffer");
			s_RendererAPI->DrawIndexed(primitive, vertexArray, instanceCount);
		}
		inline static void DrawEmpty(const Primitive primitive, uint32_t vertexCount, 
			uint32_t instanceCount = 1)
		{
			s_RendererAPI->DrawEmpty(primitive, vertexCount, instanceCount);
		}

		static void DispatchCompute(Ref<ComputeShader> shader, uint32_t x, uint32_t y, uint32_t z);
		inline static void MemoryBarrier(GLbitfield barriers) { glMemoryBarrier(barriers); }

	private:
		static RendererAPI* s_RendererAPI;
		static bool s_DepthTest;
		static bool s_BlendColor;
	};
}

