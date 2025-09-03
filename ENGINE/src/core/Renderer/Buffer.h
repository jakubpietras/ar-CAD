#pragma once
#include <vector>
#include "VertexTypes.h"

namespace ar
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}
		virtual void Bind(uint32_t vao, uint32_t bindingIndex, uint32_t attribStartIndex) = 0;
		virtual void UpdateSubData(const void* data, size_t offset, size_t size, size_t vertexCount) = 0;
		virtual void UpdateData(const void* data, size_t size, size_t vertexCount) = 0;

		inline const BufferLayout GetLayout() const { return m_Layout; }
		inline const uint32_t GetAttribCount() const { return m_Layout.GetAttribCount(); }
		inline const size_t GetVertexCount() const { return m_VertexCount; }

		static VertexBuffer* Create(std::vector<VertexPosition> vertices);
		static VertexBuffer* Create(std::vector<VertexPositionID> vertices);
		static VertexBuffer* Create(std::vector<VertexPositionColor> vertices);
		static VertexBuffer* Create(std::vector<VertexPositionIDColor> vertices);
		static VertexBuffer* Create(std::vector<InstancedFloat3> vertices);
		static VertexBuffer* Create(std::vector<InstancedMat4> vertices);
		// New overloads for Create() for each defined Vertex type (change in the future)

	protected:
		uint32_t m_ID;
		size_t m_VertexCount, m_InstanceCount;
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}
		virtual void Bind(uint32_t vao) = 0;
		virtual void UpdateSubData(const void* data, size_t offset, size_t size, size_t count) = 0;
		virtual void UpdateData(const void* data, size_t size, size_t count) = 0;

		inline const size_t GetCount() const { return m_Count; }

		static IndexBuffer* Create(std::vector<uint32_t> indices);
		static std::vector<std::shared_ptr<IndexBuffer>> Create(std::vector<std::vector<uint32_t>> indices);

	protected:
		uint32_t m_ID;
		size_t m_Count;
	};

}

