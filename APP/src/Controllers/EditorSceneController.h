#pragma once
#include "EditorState.h"
#include "ARCAD.h"

class EditorSceneController
{
public:
	EditorSceneController(ar::Ref<ar::Scene> scene);

	inline ar::Ref<ar::PerspectiveCamera> GetCamera() { return m_CameraController->GetCamera(); }
	inline ar::Ref<ar::CameraController> GetCameraController() { return m_CameraController; }
	inline void OnUpdateCamera() { m_CameraController->OnUpdate(); }
	inline void OnEventCamera(ar::Event& event) { m_CameraController->OnEvent(event); }
	void ProcessStateChanges(EditorState& state);

private:
	ar::Ref<ar::Scene> m_Scene;
	ar::Ref<ar::CameraController> m_CameraController;

	void PlaceCursor(ar::mat::Vec3 clickPosition, ViewportSize viewport, ar::mat::Vec3& cursorPosition);

	void DeleteEntities(std::vector<ar::Entity>& entities);

	void AddPoint(ar::mat::Vec3 spawnPoint);
	void AddTorus(ar::mat::Vec3 spawnPoint, ar::TorusDesc desc);
	void AddChain(std::vector<ar::Entity> points);

};