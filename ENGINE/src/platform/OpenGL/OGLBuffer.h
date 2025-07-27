#pragma once
#include "core/Renderer/Buffer.h"
#include "core/Renderer/BufferLayout.h"

namespace ar
{
	class OGLVertexBuffer : public VertexBuffer
	{
	public:
		OGLVertexBuffer(const void* data, unsigned int size, unsigned int vertexCount,
			BufferLayout layout);
		~OGLVertexBuffer();
		void Bind(uint32_t vao, uint32_t bindingIndex, uint32_t attribStartIndex) override;

	};

	class OGLIndexBuffer : public IndexBuffer
	{
	public:
		OGLIndexBuffer(const void* data, unsigned int size, uint32_t count);
		~OGLIndexBuffer();
		void Bind(uint32_t vao) override;
	};
}
