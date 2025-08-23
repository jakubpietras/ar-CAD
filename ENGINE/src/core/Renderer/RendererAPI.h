#pragma once
#include <ARMAT.h>
#include <memory>
#include "VertexArray.h"
#include "Primitive.h"

namespace ar
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		virtual void Clear() = 0;
		virtual void SetClearColor(mat::Vec4 color) = 0;

		virtual void Draw(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount = 1) = 0;

		virtual void DrawIndexed(const Primitive primitive,
			const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount = 1) = 0;

		virtual void DrawIndexedOverride(const Primitive primitive, const std::shared_ptr<VertexArray>& vertexArray,
			std::shared_ptr<IndexBuffer> ibOverride, uint32_t instanceCount = 1) = 0;

		virtual void DrawEmpty(const Primitive primitive, uint32_t vertexCount,
			uint32_t instanceCount = 1) = 0;


		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

}

