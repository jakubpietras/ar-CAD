#pragma once
#include <stdint.h>
#include <vector>

#include "Buffer.h"
#include <memory>

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
		inline const bool IsIndexed() { return m_IndexBuffers.size() > 0; }
		virtual const uint32_t GetIndexCount() = 0;
		virtual const uint32_t GetVertexCount() = 0;

		virtual const std::vector<std::shared_ptr<IndexBuffer>>& GetIndexBuffers() = 0;

		const uint32_t GetID() { return m_ID; }

	protected:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::vector<std::shared_ptr<IndexBuffer>> m_IndexBuffers;
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