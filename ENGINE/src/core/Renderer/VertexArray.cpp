#include "arpch.h"
#include "VertexArray.h"
#include "platform/OpenGL/OGLVertexArray.h"

namespace ar
{

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
			{
				return nullptr;
			}
			case RendererAPI::OpenGL:
			{
				return new OGLVertexArray();
			}
		default:
			return nullptr;
		}
	}

}