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
		
		const size_t GetIndexCount() override;
		const size_t GetVertexCount() override;
	};

}

