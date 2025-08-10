#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

class EditorCursor
{
public:
	EditorCursor();

	// Rendering
	void Render(ar::Ref<ar::CameraController> camera, float viewportHeight, ar::mat::Vec3 position);

private:
	static const float CURSOR_SIZE;

	ar::Ref<ar::VertexArray> m_Mesh;
	ar::mat::Mat4 m_ModelMatrix;
};

