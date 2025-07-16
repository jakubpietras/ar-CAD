#include "arpch.h"
#include "OGLRendererAPI.h"

#include <glad/glad.h>

namespace ar
{
	void OGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OGLRendererAPI::SetClearColor(Vec4 color)
	{
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void OGLRendererAPI::Draw(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexCount());
	}

	void OGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
	}
}

