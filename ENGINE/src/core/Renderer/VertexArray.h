#pragma once
#include "core/Core.h"
#include <stdint.h>
#include <vector>

#include "Buffer.h"
#include <memory>
#include "core/Renderer/Texture.h"

namespace ar
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() { }
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void BindTextures();

		virtual void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;
		virtual void AddIndexBuffer(Ref<IndexBuffer> indexBuffer) = 0;
		virtual void AddTexture(Ref<Texture> texture) = 0;
		virtual void ClearBuffers() = 0;
		virtual void ClearTextures() = 0;
		virtual void ClearVertexBuffers() = 0;

		static VertexArray* Create();
		inline const bool IsIndexed() { return m_IndexBuffers.size() > 0; }
		virtual const size_t GetIndexCount() = 0;
		virtual const size_t GetVertexCount() = 0;

		const uint32_t GetID() { return m_ID; }
		inline std::vector<Ref<VertexBuffer>>& GetVertexBuffers() { return m_VertexBuffers; }
		inline std::vector<Ref<IndexBuffer>>& GetIndexBuffers() { return m_IndexBuffers; }


	protected:
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		std::vector<Ref<IndexBuffer>> m_IndexBuffers;
		std::vector<Ref<Texture>> m_Textures;
		uint32_t m_ID;
		uint32_t m_BindingIndex;
		uint32_t m_AttribStartIndex;
		uint32_t m_TextureSlot;
		/*
			BindingIndex is the next available index for a VBO to bind. It needs to be incremented
			every time a new vertex buffer is added. It's not an optimal solution, but for now it
			will do. Texture slot is the same.
		*/
	};
}