#pragma once
#include "core/Renderer/Buffer.h"
#include "core/Renderer/BufferLayout.h"

namespace ar
{
	class OGLVertexBuffer : public VertexBuffer
	{
	public:
		OGLVertexBuffer(const void* data, unsigned int size, BufferLayout layout);
		~OGLVertexBuffer();
		void Bind(uint32_t vao, uint32_t bindingIndex) override;
		const BufferLayout GetLayout() const override;
	};

	class OGLIndexBuffer : public IndexBuffer
	{
	public:
		OGLIndexBuffer(const void* data, unsigned int size);
		~OGLIndexBuffer();
		void Bind(uint32_t vao) override;
		const uint32_t GetCount() const override;
	};
}
