#pragma once
#include "core/Renderer/VertexArray.h"


namespace ar
{
	class OGLVertexArray : public VertexArray
	{
	public:
		OGLVertexArray();

		void Bind() override;
		void Unbind() override;
		void AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) override;
		void AddIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) override;
	};

}

