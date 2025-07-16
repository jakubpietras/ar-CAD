#include "arpch.h"
#include "VertexArray.h"
#include "platform/OpenGL/OGLVertexArray.h"
#include "RendererAPI.h"

namespace ar
{
	VertexArray* VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
			{
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return new OGLVertexArray();
			}
		default:
			return nullptr;
		}
	}

}