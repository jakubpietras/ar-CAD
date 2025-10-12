#include <glad/glad.h>
#include "arpch.h"
#include "OGLRendererAPI.h"
#include "core/Renderer/Primitive.h"


namespace ar
{
	void OGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OGLRendererAPI::SetClearColor(mat::Vec4 color)
	{
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void OGLRendererAPI::Draw(const Primitive primitive,
		const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount)
	{
		glDrawArraysInstanced(GetOGLPrimitive(primitive), 0, static_cast<GLsizei>(vertexArray->GetVertexCount()),
			instanceCount);
	}

	void OGLRendererAPI::DrawIndexed(const Primitive primitive,
		const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount)
	{
		for (auto& ib : vertexArray->GetIndexBuffers())
		{
			ib->Bind(vertexArray->GetID());
			glDrawElementsInstanced(GetOGLPrimitive(primitive), static_cast<GLsizei>(ib->GetCount()),
				GL_UNSIGNED_INT, nullptr, instanceCount);
		}
		
	}

	void OGLRendererAPI::DrawEmpty(const Primitive primitive, uint32_t vertexCount,
		uint32_t instanceCount)
	{
		glDrawArraysInstanced(GetOGLPrimitive(primitive), 0, vertexCount, instanceCount);
	}

	GLenum OGLRendererAPI::GetOGLPrimitive(Primitive primitive)
	{
		switch (primitive)
		{
		case Primitive::Line: return GL_LINES;
		case Primitive::LineLoop: return GL_LINE_LOOP;
		case Primitive::LineStrip: return GL_LINE_STRIP;
		case Primitive::Point: return GL_POINTS;
		case Primitive::Triangle: return GL_TRIANGLES;
		case Primitive::TriangleStrip: return GL_TRIANGLE_STRIP;
		case Primitive::Patch: return GL_PATCHES;
		default:
			return 0;
		}
	}
}

