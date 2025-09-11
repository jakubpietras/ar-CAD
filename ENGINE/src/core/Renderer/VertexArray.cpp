#include "arpch.h"
#include "VertexArray.h"
#include "platform/OpenGL/OGLVertexArray.h"
#include "RendererAPI.h"

namespace ar
{

	void VertexArray::BindTextures()
	{
		if (m_Textures.empty())
			return;
		size_t i = 0;
		for (auto& texture : m_Textures)
			texture->BindSlot(i);
	}

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