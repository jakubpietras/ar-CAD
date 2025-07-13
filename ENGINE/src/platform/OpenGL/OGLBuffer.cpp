#include "arpch.h"
#include "OGLBuffer.h"

namespace ar
{
	OGLVertexBuffer::OGLVertexBuffer(const void* data, unsigned int size, std::vector<Attribute> layout)
	{
		glCreateBuffers(1, &m_ID);
		int status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL CreateBuffers failed. ");

		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL NamedBufferData failed. ");

		SetLayout(layout);
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

	const std::vector<ar::Attribute> OGLVertexBuffer::GetLayout() const
	{
		return m_Layout;
	}

	void OGLVertexBuffer::SetLayout(std::vector<Attribute> layout)
	{
		m_Layout = layout;
	}

	OGLIndexBuffer::OGLIndexBuffer(const void* data, unsigned int size)
	{
		glCreateBuffers(1, &m_ID);
		int status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL CreateBuffers failed. ");

		glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
		status = CheckGLErrors();
		AR_ASSERT(!status, "OpenGL NamedBufferData failed. ");
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
