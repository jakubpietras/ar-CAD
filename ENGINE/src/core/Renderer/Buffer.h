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
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetLayout(std::vector<Attribute> layout) = 0;
		virtual const std::vector<Attribute> GetLayout() const = 0;

		static VertexBuffer* Create(std::vector<VertexPosition> vertices);
		// New overloads for Create if more Vertex types are created (fix in the future)

	protected:
		uint32_t m_ID;
		std::vector<Attribute> m_Layout;
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual const uint32_t GetCount() const = 0;

		static IndexBuffer* Create(std::vector<unsigned int> vertices);

	protected:
		uint32_t m_ID;
		uint32_t m_Count;
	};

}

