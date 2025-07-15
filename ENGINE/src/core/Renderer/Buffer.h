#pragma once
#include <vector>
#include <glad/glad.h>

#include "Renderer.h"
#include "VertexTypes.h"

namespace ar
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}
		virtual void Bind(uint32_t vao, uint32_t bindingIndex, uint32_t attribStartIndex) = 0;
		virtual const BufferLayout GetLayout() const = 0;
		virtual const uint32_t GetAttribCount() = 0;

		static VertexBuffer* Create(std::vector<VertexPosition> vertices);
		static VertexBuffer* Create(std::vector<VertexPositionColor> vertices);
		// New overloads for Create() for each defined Vertex type (change in the future)

	protected:
		uint32_t m_ID;
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}
		virtual void Bind(uint32_t vao) = 0;
		virtual const uint32_t GetCount() const = 0;

		static IndexBuffer* Create(std::vector<unsigned int> indices);

	protected:
		uint32_t m_ID;
		uint32_t m_Count;
	};

}

