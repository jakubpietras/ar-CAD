#pragma once
#include <vector>
#include <glad/glad.h>

#include "Renderer.h"
#include "VertexTypes.h"

class OGLVertexBuffer;
class OGLIndexBuffer;

namespace ar
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetLayout(std::vector<Attribute> layout) = 0;
		virtual const std::vector<Attribute> GetLayout() const = 0;

		template <typename T>
		inline static VertexBuffer* Create(std::vector<T> vertices)
		{
			switch (Renderer::GetAPI())
			{
				case RendererAPI::None:
				{
					return nullptr;
				}
				case RendererAPI::OpenGL:
				{
					return new OGLVertexBuffer(vertices.data(), vertices.size() * sizeof(T));
				}
			}
		}

	protected:
		uint32_t m_ID;
		std::vector<Attribute> m_Layout;
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual const uint32_t GetCount() const = 0;

		template <typename T>
		static IndexBuffer* Create(std::vector<T> vertices);

	protected:
		uint32_t m_ID;
		uint32_t m_Count;
	};

}

