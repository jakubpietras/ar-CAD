#include "arpch.h"
#include "OGLVertexArray.h"

namespace ar
{

	OGLVertexArray::OGLVertexArray()
	{
		glCreateVertexArrays(1, &m_ID);
		AR_GL_CHECK();
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

	void OGLVertexArray::AddVertexBuffer(Ref<VertexBuffer> vertexBuffer)
	{
		m_VertexBuffers.push_back(vertexBuffer);
		vertexBuffer->Bind(m_ID, m_BindingIndex, m_AttribStartIndex);
		m_AttribStartIndex += vertexBuffer->GetAttribCount();
		m_BindingIndex++;
		AR_ASSERT(m_BindingIndex < GL_MAX_VERTEX_ATTRIB_BINDINGS, 
			"Max vertex attribute bindings reached!");
	}

	void OGLVertexArray::AddIndexBuffer(Ref<IndexBuffer> indexBuffer)
	{
		m_IndexBuffers.push_back(indexBuffer);
		// binding should occur during rendering, each index buffer is a separate draw call
		//indexBuffer->Bind(m_ID);
	}

	const uint32_t OGLVertexArray::GetIndexCount()
	{
		/*AR_ASSERT(m_IndexBuffer, "Index buffer not bound!");
		return m_IndexBuffer->GetCount();*/
		return 0;
	}

	const uint32_t OGLVertexArray::GetVertexCount()
	{
		uint32_t vertexCount = 0;
		for (auto vb : m_VertexBuffers)
			vertexCount += vb->GetVertexCount();
		return vertexCount;
	}

	void OGLVertexArray::ClearBuffers()
	{
		for (uint32_t i = 0; i < m_AttribStartIndex; ++i)
		{
			glDisableVertexArrayAttrib(m_ID, i);
		}
		glVertexArrayElementBuffer(m_ID, 0);

		m_VertexBuffers.clear();
		m_IndexBuffers.clear();
		m_BindingIndex = 0;
		m_AttribStartIndex = 0;
	}

}

