#include "arpch.h"
#include "OGLVertexArray.h"

namespace ar
{

	OGLVertexArray::OGLVertexArray()
	{
		glCreateVertexArrays(1, &m_ID);
		CheckGLErrors();
		m_BindingIndex = 0;
	}

	OGLVertexArray::~OGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_ID);
	}

	void OGLVertexArray::Bind()
	{
		glBindVertexArray(m_ID);
	}

	void OGLVertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void OGLVertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer)
	{
		m_VertexBuffers.push_back(vertexBuffer);
		vertexBuffer->Bind(m_ID, m_BindingIndex, m_AttribStartIndex);
		m_AttribStartIndex += vertexBuffer->GetAttribCount();
		m_BindingIndex++;
		AR_ASSERT(m_BindingIndex < GL_MAX_VERTEX_ATTRIB_BINDINGS, 
			"Max vertex attribute bindings reached!");
	}

	void OGLVertexArray::AddIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
	{
		m_IndexBuffer = indexBuffer;
		indexBuffer->Bind(m_ID);
	}
}

