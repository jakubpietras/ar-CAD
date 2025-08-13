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

	ar::VertexBuffer* VertexBuffer::Create(std::vector<InstancedFloat3> vertices)
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
				static_cast<unsigned int>(vertices.size()) * sizeof(InstancedFloat3),
				static_cast<unsigned int>(vertices.size()), InstancedFloat3::s_Layout);
		}
		default:
			return nullptr;
		}
	}

	ar::VertexBuffer* VertexBuffer::Create(std::vector<InstancedMat4> matrices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
		{
			return nullptr;
		}
		case RendererAPI::API::OpenGL:
		{
			return new OGLVertexBuffer(matrices.data(),
				static_cast<unsigned int>(matrices.size()) * sizeof(InstancedMat4),
				static_cast<unsigned int>(matrices.size()), InstancedMat4::s_Layout);
		}
		default:
			return nullptr;
		}
	}

	ar::VertexBuffer* VertexBuffer::Create(std::vector<VertexPositionID> vertices)
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
				static_cast<unsigned int>(vertices.size()) * sizeof(VertexPositionID),
				static_cast<unsigned int>(vertices.size()), VertexPositionID::s_Layout);
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
					static_cast<unsigned int>(indices.size()) * sizeof(unsigned int), indices.size());
			}
			default:
				return nullptr;
		}
	}

	std::vector<std::shared_ptr<IndexBuffer>> IndexBuffer::Create(std::vector<std::vector<uint32_t>> indices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
		{
			return {};
		}
		case RendererAPI::API::OpenGL:
		{
			std::vector<std::shared_ptr<IndexBuffer>> ibs;

			for (auto& ind : indices)
			{
				auto buffer = std::make_shared<OGLIndexBuffer>(ind.data(),
					static_cast<unsigned int>(ind.size()) * sizeof(unsigned int), ind.size());
				ibs.push_back(buffer);
			}
			return ibs;
		}
		default:
			return {};
		}
	}

}