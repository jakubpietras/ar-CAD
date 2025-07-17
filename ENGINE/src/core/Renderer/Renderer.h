#pragma once
#include "VertexArray.h"
#include "Shader.h"

namespace ar
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
	};
}