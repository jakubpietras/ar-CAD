#pragma once
#include "core/Renderer/Buffer.h"

namespace ar
{
	class OGLVertexBuffer : public ar::VertexBuffer
	{
	public:
		OGLVertexBuffer(const void* data, unsigned int size);
		~OGLVertexBuffer();
		void Bind() const override;
		void Unbind() const override;
		const std::vector<Attribute> GetLayout() const override;
	protected:
		void SetLayout(std::vector<Attribute> layout);
	};

	class OGLIndexBuffer : public ar::IndexBuffer
	{
	public:
		OGLIndexBuffer(const void* data, unsigned int size);
		~OGLIndexBuffer();
		void Bind() const override;
		void Unbind() const override;
		const uint32_t GetCount() const override;
	};
}
