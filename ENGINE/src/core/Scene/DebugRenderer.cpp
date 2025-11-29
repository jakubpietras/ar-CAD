#include "arpch.h"
#include "DebugRenderer.h"
#include "core/Renderer/Shader.h"
#include "core/Renderer/Renderer.h"
#include <vector>
#include "core/Renderer/RenderCommand.h"

namespace ar
{
	ar::Ref<ar::VertexArray> DebugRenderer::m_PointsVA = nullptr;
	ar::Ref<ar::VertexArray> DebugRenderer::m_LinesVA = nullptr;
	std::vector<ar::VertexPositionColor> DebugRenderer::m_Points{};
	std::vector<ar::VertexPositionColor> DebugRenderer::m_Lines{};
	bool DebugRenderer::m_ShouldResetBuffers = false;

	void DebugRenderer::Init()
	{
		m_PointsVA = Ref<VertexArray>(VertexArray::Create());
		m_LinesVA = Ref<VertexArray>(VertexArray::Create());
	}

	void DebugRenderer::Render(ar::mat::Mat4 viewProjection)
	{
		if (m_ShouldResetBuffers)
		{
			m_PointsVA->ClearBuffers();
			m_PointsVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(m_Points)));
			
			m_LinesVA->ClearBuffers();
			m_LinesVA->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(m_Lines)));

			m_ShouldResetBuffers = false;
		}

		auto shader = ar::ShaderLib::Get("BasicColor");
		shader->SetMat4("u_VP", viewProjection);
		shader->SetMat4("u_Model", mat::Identity());

		if (!m_Points.empty())
		{
			ar::Renderer::Submit(Primitive::Point, shader, m_PointsVA);
		}
		if (!m_Lines.empty())
			ar::Renderer::Submit(Primitive::Line, shader, m_LinesVA);
	}

	void DebugRenderer::Clear()
	{
		m_Points.clear();
		m_Lines.clear();
		m_PointsVA->ClearBuffers();
		m_LinesVA->ClearBuffers();
	}

	void DebugRenderer::AddPoint(mat::Vec3 position, mat::Vec3 color)
	{
		m_Points.push_back({ position, color });
		m_ShouldResetBuffers = true;
	}

	void DebugRenderer::AddLine(mat::Vec3 end1, mat::Vec3 end2)
	{
		m_Lines.push_back({ end1, DebugRenderer::LINE_COLOR });
		m_Lines.push_back({ end2, DebugRenderer::LINE_COLOR });
		m_ShouldResetBuffers = true;
	}

}