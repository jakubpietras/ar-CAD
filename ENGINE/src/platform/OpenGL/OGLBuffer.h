#pragma once
#include "core/Renderer/Buffer.h"

namespace ar
{
	class OGLVertexBuffer : public VertexBuffer
	{
	public:
		OGLVertexBuffer(const void* data, unsigned int size, std::vector<Attribute> layout);
		~OGLVertexBuffer();
		void Bind() const override;
		void Unbind() const override;
		const std::vector<Attribute> GetLayout() const override;
	protected:
		void SetLayout(std::vector<Attribute> layout);
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
