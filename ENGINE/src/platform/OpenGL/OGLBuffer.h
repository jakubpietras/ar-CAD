#pragma once
#include "core/Renderer/Buffer.h"
#include "core/Renderer/BufferLayout.h"

namespace ar
{
	class OGLVertexBuffer : public VertexBuffer
	{
	public:
		OGLVertexBuffer(const void* data, size_t size, size_t vertexCount,
			BufferLayout layout);
		~OGLVertexBuffer();
		void Bind(uint32_t vao, uint32_t bindingIndex, uint32_t attribStartIndex) override;
		void UpdateSubData(const void* data, size_t offset, size_t size, size_t vertexCount) override;
		void UpdateData(const void* data, size_t size, size_t vertexCount) override;
	};

	class OGLIndexBuffer : public IndexBuffer
	{
	public:
		OGLIndexBuffer(const void* data, size_t size, size_t count);
		~OGLIndexBuffer();
		void Bind(uint32_t vao) override;
		void UpdateSubData(const void* data, size_t offset, size_t size, size_t count) override;
		void UpdateData(const void* data, size_t size, size_t count) override;
	};
}
