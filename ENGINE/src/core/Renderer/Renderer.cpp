#include "arpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

namespace ar
{
	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader,
		const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->Use();
		vertexArray->Bind();
		if (vertexArray->GetIndexBuffer())
		{
			RenderCommand::DrawIndexed(vertexArray);
		}
		else
			RenderCommand::Draw(vertexArray);
		vertexArray->Unbind();
	}
}