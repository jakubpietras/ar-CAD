#include "SimRenderer.h"

SimRenderer::SimRenderer()
	: m_Framebuffer(std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create({ 1920, 1080 })))
{ }

void SimRenderer::OnResize(ar::mat::Vec2 newVP)
{
	m_Framebuffer->Resize(static_cast<uint32_t>(newVP.x), static_cast<uint32_t>(newVP.y));
}

void SimRenderer::Render(const ar::Ref<ar::CameraController>& cameraController, ar::mat::Vec2 viewport)
{
	m_Framebuffer->Bind();
	ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.18f, 0.18f, 0.24f, 1.0f));
	ar::RenderCommand::Clear();
	ar::RenderCommand::ToggleDepthTest(true);
	ar::RenderCommand::ToggleBlendColor(true);
	
	auto& vpMat = cameraController->GetCamera()->GetVP();
	RenderGrid(vpMat);
	RenderMaterial(vpMat);
	RenderCutter(vpMat);
	RenderPaths(vpMat);
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

void SimRenderer::RenderMaterial(ar::mat::Mat4 vp)
{

}

void SimRenderer::RenderCutter(ar::mat::Mat4 vp)
{

}

void SimRenderer::RenderPaths(ar::mat::Mat4 vp)
{

}
