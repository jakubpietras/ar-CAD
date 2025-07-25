#pragma once
#include <vector>
#include <glad/glad.h>
#include "VertexTypes.h"

namespace ar
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}
		virtual void Bind(uint32_t vao, uint32_t bindingIndex, uint32_t attribStartIndex) = 0;
		
		inline const BufferLayout GetLayout() const { return m_Layout; }
		inline const uint32_t GetAttribCount() const { return m_Layout.GetAttribCount(); }
		inline const uint32_t GetVertexCount() const { return m_VertexCount; }

		static VertexBuffer* Create(std::vector<VertexPosition> vertices);
		static VertexBuffer* Create(std::vector<VertexPositionColor> vertices);
		static VertexBuffer* Create(std::vector<InstancedFloat3> vertices);
		static VertexBuffer* Create(std::vector<InstancedMat4> vertices);



		// New overloads for Create() for each defined Vertex type (change in the future)

	protected:
		uint32_t m_ID, m_VertexCount;
		uint32_t m_InstanceCount;
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}
		virtual void Bind(uint32_t vao) = 0;
		
		inline const uint32_t GetCount() const { return m_Count; }

		static IndexBuffer* Create(std::vector<unsigned int> indices);

	protected:
		uint32_t m_ID, m_Count;
	};

}

