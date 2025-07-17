#include "arpch.h"
#include "Buffer.h"
#include "platform/OpenGL/OGLBuffer.h"
#include "core/Renderer/RendererAPI.h"

namespace ar
{
	VertexBuffer* VertexBuffer::Create(std::vector<VertexPosition> vertices)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
			{
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return new OGLVertexBuffer(vertices.data(),
					static_cast<unsigned int>(vertices.size()) * sizeof(VertexPosition),
					static_cast<unsigned int>(vertices.size()), VertexPosition::s_Layout);
			}
			default:
				return nullptr;
		}
	}

	ar::VertexBuffer* VertexBuffer::Create(std::vector<VertexPositionColor> vertices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
		{
			return nullptr;
		}
		case RendererAPI::API::OpenGL:
		{
			return new OGLVertexBuffer(vertices.data(),
				static_cast<unsigned int>(vertices.size()) * sizeof(VertexPositionColor), 
				static_cast<unsigned int>(vertices.size()), VertexPositionColor::s_Layout);
		}
		default:
			return nullptr;
		}
	}

	ar::IndexBuffer* IndexBuffer::Create(std::vector<unsigned int> indices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			{
				return nullptr;
			}
		case RendererAPI::API::OpenGL:
			{
				return new OGLIndexBuffer(indices.data(), 
					static_cast<unsigned int>(indices.size()) * sizeof(unsigned int));
			}
			default:
				return nullptr;
		}
	}

}