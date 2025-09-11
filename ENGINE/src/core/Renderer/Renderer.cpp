#include "arpch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "core/Scene/Components.h"

namespace ar
{
	std::unique_ptr<VertexArray> Renderer::s_DummyVAO = nullptr;

	void Renderer::BeginScene()
	{

	}
	void Renderer::EndScene()
	{
	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::Submit(const Primitive primitive, const std::shared_ptr<Shader>& shader,
		const std::shared_ptr<VertexArray>& vertexArray, bool indexed, uint32_t instanceCount)
	{
		shader->Use();
		vertexArray->Bind();
		vertexArray->BindTextures();
		if (indexed)
		{
			RenderCommand::DrawIndexed(primitive, vertexArray, instanceCount);
		}
		else
			RenderCommand::Draw(primitive, vertexArray, instanceCount);
		vertexArray->Unbind();
	}

	void Renderer::Submit(const Primitive primitive, const std::shared_ptr<Shader>& shader,
		uint32_t vertexCount, bool indexed, uint32_t instanceCount)
	{
		// Should be used if data is defined inside the shader (e.g. a quad)
		if (!s_DummyVAO)
			s_DummyVAO = std::unique_ptr<ar::VertexArray>(ar::VertexArray::Create());
		
		shader->Use();
		s_DummyVAO->Bind();
		RenderCommand::DrawEmpty(primitive, vertexCount, instanceCount);
		s_DummyVAO->Unbind();
	}

	void Renderer::Submit(MeshComponent& mesh, bool indexed, uint32_t instanceCount /*= 1*/)
	{
		auto shader = mesh.GetShader();

		shader->Use();
		mesh.VertexArray->Bind();

		if (mesh.RenderPrimitive == Primitive::Patch)
			RenderCommand::SetTessellationPatchSize(mesh.TessellationPatchSize);

		if (indexed)
			RenderCommand::DrawIndexed(mesh.RenderPrimitive, mesh.VertexArray, instanceCount);
		else
			RenderCommand::Draw(mesh.RenderPrimitive, mesh.VertexArray, instanceCount);
		mesh.VertexArray->Unbind();
	}

}