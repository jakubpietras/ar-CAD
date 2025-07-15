#include "arpch.h"
#include "OGLBuffer.h"

namespace ar
{
	OGLVertexBuffer::OGLVertexBuffer(const void* data, unsigned int size, BufferLayout layout)
	{
		glCreateBuffers(1, &m_ID);
		CheckGLErrors();

		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		CheckGLErrors();

		m_Layout = layout;
	}

	OGLVertexBuffer::~OGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void OGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	}

	void OGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OGLVertexBuffer::EnableLayout()
	{
		uint32_t index = 0;

		for (auto& attribute : m_Layout)
		{
			glVertexAttribPointer(
				index,
				attribute.GetCount(),
				attribute.GetOpenGLType(),
				attribute.Normalized ? GL_TRUE : GL_FALSE,
				m_Layout.GetStride(),
				reinterpret_cast<const void*>(attribute.Offset)
			);
			glEnableVertexAttribArray(index);
			index++;
		}
	}

	const ar::BufferLayout OGLVertexBuffer::GetLayout() const
	{
		return m_Layout;
	}

	OGLIndexBuffer::OGLIndexBuffer(const void* data, unsigned int size)
	{
		glCreateBuffers(1, &m_ID);
		int status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL CreateBuffers failed. ");

		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL NamedBufferData failed. ");

		m_Count = size;
	}

	OGLIndexBuffer::~OGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void OGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	}

	void OGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	const uint32_t OGLIndexBuffer::GetCount() const
	{
		return m_Count;
	}

}
