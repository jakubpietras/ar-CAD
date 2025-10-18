#include "SimRenderer.h"

SimRenderer::SimRenderer()
	: m_Framebuffer(std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create({ 1920, 1080 })))
{ }

void SimRenderer::OnResize(ar::mat::Vec2 newVP)
{
	m_Framebuffer->Resize(static_cast<uint32_t>(newVP.x), static_cast<uint32_t>(newVP.y));
}

void SimRenderer::Render(ar::mat::Mat4 vp)
{
	m_Framebuffer->Bind();
	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
	ar::RenderCommand::Clear();
	ar::RenderCommand::ToggleDepthTest(true);
	ar::RenderCommand::ToggleBlendColor(true);

	RenderGrid(vp);
	RenderCutter(vp);
	m_Framebuffer->Unbind();
}

void SimRenderer::RenderGrid(ar::mat::Mat4 vp)
{
	auto gridShader = ar::ShaderLib::Get("Grid");
	ar::RenderCommand::ToggleDepthTest(false);
	gridShader->SetMat4("u_VP", vp);
	ar::Renderer::Submit(ar::Primitive::Triangle, gridShader, 6);
	ar::RenderCommand::ToggleDepthTest(true);
}

void SimRenderer::RenderMaterial(ar::mat::Mat4 vp, ar::mat::Vec3 cameraPos, MillingStock& block,
	ar::Ref<ar::Texture> heightMap)
{
	m_Framebuffer->Bind();
	block.Render(vp, cameraPos, heightMap);
	m_Framebuffer->Unbind();
}

void SimRenderer::RenderCutter(ar::mat::Mat4 vp)
{
	// TODOdsddjsxdddsasssdddss
}

void SimRenderer::RenderPaths(ar::Ref<ar::VertexArray> path, ar::mat::Mat4 vp)
{
	m_Framebuffer->Bind();
	auto shader = ar::ShaderLib::Get("Paths");
	shader->SetMat4("u_VP", vp);
	shader->SetMat4("u_Model", ar::mat::RotationMatrix({-90.0f, 0.0f, 0.0f}));
	shader->SetVec3("u_Color", {0.f, 1.f, 0.f});
	ar::Renderer::Submit(ar::Primitive::LineStrip, shader, path, false);
	m_Framebuffer->Unbind();
}
