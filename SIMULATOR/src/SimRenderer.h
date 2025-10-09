#pragma once
#include <ARMAT.h>
#include <ARCAD.h>
#include "core/CameraController.h"

class SimRenderer
{
public:
	SimRenderer();
	void OnResize(ar::mat::Vec2 newVP);
	void Render(ar::mat::Mat4 vp);
	void RenderPaths(ar::Ref<ar::VertexArray> path, ar::mat::Mat4 vp);

	ar::Ref<ar::Framebuffer> GetFramebuffer() const { return m_Framebuffer; }
private:
	ar::Ref<ar::Framebuffer> m_Framebuffer;

	void RenderGrid(ar::mat::Mat4 vp);
	void RenderMaterial(ar::mat::Mat4 vp);
	void RenderCutter(ar::mat::Mat4 vp);
};

