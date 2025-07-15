#include "arpch.h"
#include "Buffer.h"
#include "platform/OpenGL/OGLBuffer.h"

namespace ar
{
	VertexBuffer* VertexBuffer::Create(std::vector<VertexPosition> vertices)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
			{
				return nullptr;
			}
			case RendererAPI::OpenGL:
			{
				return new OGLVertexBuffer(vertices.data(),
					static_cast<unsigned int>(vertices.size()) * sizeof(VertexPosition),
					VertexPosition::s_Layout);
			}
			default:
				return nullptr;
		}
	}

	ar::IndexBuffer* IndexBuffer::Create(std::vector<unsigned int> indices)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
			{
				return nullptr;
			}
			case RendererAPI::OpenGL:
			{
				return new OGLIndexBuffer(indices.data(), 
					static_cast<unsigned int>(indices.size()) * sizeof(unsigned int));
			}
			default:
				return nullptr;
		}
	}

}