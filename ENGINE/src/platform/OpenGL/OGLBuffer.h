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
		void Bind() const override;
		void Unbind() const override;
		void EnableLayout() override;
		const BufferLayout GetLayout() const override;
	};

	class OGLIndexBuffer : public IndexBuffer
	{
	public:
		OGLIndexBuffer(const void* data, unsigned int size);
		~OGLIndexBuffer();
		void Bind() const override;
		void Unbind() const override;
		const uint32_t GetCount() const override;
	};
}
