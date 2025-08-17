#pragma once
#include "EditorState.h"
#include "ARCAD.h"
#include "LogUtils.h"
#include "core/Utils/CurveUtils.h"
#include "EditorUI.h"
#include "core/Scene/SceneRenderer.h"

class EditorSceneController
{
public:
	EditorSceneController(ar::Ref<ar::Scene> scene, ar::SceneRenderer& sceneRender);

	inline ar::Ref<ar::PerspectiveCamera> GetCamera() { return m_CameraController->GetCamera(); }
	inline ar::Ref<ar::CameraController> GetCameraController() { return m_CameraController; }
	inline void OnUpdateCamera() { m_CameraController->OnUpdate(); }
	inline void OnEventCamera(ar::Event& event) { m_CameraController->OnEvent(event); }
	void ProcessStateChanges(EditorState& state);

private:
	ar::Ref<ar::Scene> m_Scene;
	ar::Ref<ar::CameraController> m_CameraController;
	ar::SceneRenderer& m_SceneRenderer;

	// Processors
	void ProcessAdd(EditorState& state);
	void ProcessSelect(EditorState& state);
	void ProcessDetach(EditorState& state);
	void ProcessAttach(EditorState& state);
	void ProcessPicking(EditorState& state);
	void ProcessGroupTransform(EditorState& state);

	// Cursor
	void PlaceCursor(ar::mat::Vec2 clickPosition, ViewportSize viewport, ar::mat::Vec3& cursorPosition);

	// Deletion
	void DeleteEntities(std::vector<ar::Entity>& entities);

	// Addition
	ar::Entity AddPoint(ar::mat::Vec3 spawnPoint);
	void AddPointToCurves(ar::Entity point, std::vector<ar::Entity> curves);
	void AddTorus(ar::mat::Vec3 spawnPoint, ar::TorusDesc desc);
	void AddChain(std::vector<ar::Entity> points);
	void AddCurveC0(std::vector<ar::Entity> points);
	void AddCurveC2(std::vector<ar::Entity> points);

	// Selection
	void SelectEntities(std::vector<ar::Entity> entities, bool add = false);
	void DeselectEntities(std::vector<ar::Entity> entities);
	void UpdateMeanPoint(EditorState& state);
	void BeginGroupTransform(EditorState& state);
	void EndGroupTransform(EditorState& state);

	// Detach
	void DetachPoint(ar::Entity child, ar::Entity parent);

	// Validate
	void ValidateGeometry(EditorState& state);
	void ValidateSelection(EditorState& state);
};