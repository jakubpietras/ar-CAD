#pragma once
#include <ARMAT.h>
#include <memory>
#include "VertexArray.h"

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
		virtual void SetClearColor(Vec4 color) = 0;

		virtual void Draw(const std::shared_ptr<VertexArray>& vertexArray) = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

}

