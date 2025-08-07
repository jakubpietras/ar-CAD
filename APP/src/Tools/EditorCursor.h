#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

class EditorCursor
{
public:
	EditorCursor();

	// Accessors
	inline void SetPosition(const ar::mat::Vec3 position) { m_Position = position; }
	inline const ar::mat::Vec3& GetPosition() { return m_Position; }

	// Rendering
	void Render(ar::Ref<ar::CameraController> camera, float viewportHeight);

private:
	static const float CURSOR_SIZE;

	ar::mat::Vec3 m_Position;
	ar::Ref<ar::VertexArray> m_Mesh;
	ar::mat::Mat4 m_ModelMatrix;
};

