#include "EditorCursor.h"

const float EditorCursor::CURSOR_SIZE = 15.0f;

EditorCursor::EditorCursor()
	: m_ModelMatrix(ar::mat::Identity())
{
	m_Mesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
	std::vector<ar::VertexPositionColor> vertices {
		{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
	};
	m_Mesh->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(vertices)));
}

void EditorCursor::Render(ar::Ref<ar::CameraController> camera, float viewportHeight, ar::mat::Vec3 position)
{
	auto positionViewSpace = camera->GetCamera()->GetView() * ar::mat::Vec4(position, 1.0f);
	float depth = std::abs(positionViewSpace.z);
	float pixelSizeAtDepth = 2.0f * depth * tan(camera->GetFOV() * 0.5f) / viewportHeight;
	float scale = pixelSizeAtDepth * CURSOR_SIZE;

	m_ModelMatrix = 
		ar::mat::TranslationMatrix(position)
		* ar::mat::ScaleMatrix({ scale, scale, scale });

	auto shader = ar::ShaderLib::Get("BasicColor");
	shader->SetMat4("u_Model", m_ModelMatrix);
	shader->SetMat4("u_VP", camera->GetCamera()->GetVP());

	ar::RenderCommand::SetLineThickness(3);
	ar::RenderCommand::ToggleDepthTest(false);
	ar::Renderer::Submit(ar::Primitive::Line, shader, m_Mesh, 6);
	ar::RenderCommand::ToggleDepthTest(false);
	ar::RenderCommand::SetLineThickness();
}

