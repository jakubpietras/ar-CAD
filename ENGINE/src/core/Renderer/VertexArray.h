#pragma once
#include <stdint.h>
#include <vector>

#include "Buffer.h"

namespace ar
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() { }
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) = 0;
		virtual void AddIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) = 0;

		static VertexArray* Create();

	protected:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		uint32_t m_ID;
		uint32_t m_BindingIndex;
		uint32_t m_AttribStartIndex;
		/*
			BindingIndex is the next available index for a VBO to bind. It needs to be incremented
			every time a new vertex buffer is added. It's not an optimal solution, but for now it
			will do.
		*/
	};
}