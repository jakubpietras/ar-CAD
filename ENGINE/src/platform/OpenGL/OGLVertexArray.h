#pragma once
#include "core/Renderer/VertexArray.h"


namespace ar
{
	class OGLVertexArray : public VertexArray
	{
	public:
		OGLVertexArray();
		~OGLVertexArray() override;

		void Bind() override;
		void Unbind() override;
		void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
		void AddIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
		void ClearBuffers() override;
		
		const uint32_t GetIndexCount() override;
		const uint32_t GetVertexCount() override;
	};

}

