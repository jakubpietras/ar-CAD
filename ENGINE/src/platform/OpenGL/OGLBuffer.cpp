#include "arpch.h"
#include "OGLBuffer.h"

namespace ar
{
	OGLVertexBuffer::OGLVertexBuffer(const void* data, unsigned int size, unsigned int vertexCount,
		BufferLayout layout)
	{
		glCreateBuffers(1, &m_ID);
		CheckGLErrors();

		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		CheckGLErrors();

		m_VertexCount = vertexCount;
		m_Layout = layout;
	}

	OGLVertexBuffer::~OGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void OGLVertexBuffer::Bind(uint32_t vao, uint32_t bindingIndex, uint32_t attribStartIndex)
	{
		uint32_t attribIndex = attribStartIndex;

		glVertexArrayVertexBuffer(vao, bindingIndex, m_ID, 0, m_Layout.GetStride());
		for (auto& attribute : m_Layout)
		{
			glVertexArrayAttribFormat(
				vao,
				attribIndex,
				attribute.GetCount(),
				attribute.GetOpenGLType(),
				attribute.Normalized ? GL_TRUE : GL_FALSE,
				attribute.Offset
			);
			glVertexArrayAttribBinding(vao, attribIndex, bindingIndex);
			glVertexArrayBindingDivisor(vao, bindingIndex, attribute.Divisor);
			glEnableVertexArrayAttrib(vao, attribIndex);
			attribIndex++;
		}
	}


	// Use for modifying data partially or uploading new data of the same size
	void OGLVertexBuffer::UpdateSubData(const void* data, size_t offset, size_t size, size_t vertexCount)
	{
		AR_ASSERT(vertexCount == m_VertexCount, "This function will not resize the buffer!");
		glNamedBufferSubData(m_ID, offset, size, data);
		CheckGLErrors();
	}

	void OGLVertexBuffer::UpdateData(const void* data, size_t size, size_t vertexCount)
	{
		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		m_VertexCount = vertexCount;
		CheckGLErrors();
	}

	OGLIndexBuffer::OGLIndexBuffer(const void* data, unsigned int size, size_t count)
	{
		glCreateBuffers(1, &m_ID);
		int status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL CreateBuffers failed. ");

		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL NamedBufferData failed. ");

		m_Count = count;
	}

	OGLIndexBuffer::~OGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void OGLIndexBuffer::Bind(uint32_t vao)
	{
		glVertexArrayElementBuffer(vao, m_ID);
	}

	void OGLIndexBuffer::UpdateSubData(const void* data, size_t offset, size_t size, size_t count)
	{
		AR_ASSERT(count == m_Count, "This function will not resize the buffer!");
		glNamedBufferSubData(m_ID, offset, size, data);
		CheckGLErrors();
	}

	void OGLIndexBuffer::UpdateData(const void* data, size_t size, size_t count)
	{
		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		m_Count = count;
		CheckGLErrors();
	}

}
