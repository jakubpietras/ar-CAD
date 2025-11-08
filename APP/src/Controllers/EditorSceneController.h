#pragma once
#include "EditorState.h"
#include "ARCAD.h"
#include "LogUtils.h"
#include "core/Utils/CurveUtils.h"
#include "EditorUI.h"
#include "core/Scene/SceneRenderer.h"
#include "core/Scene/SceneFactory.h"

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
	ar::Entity m_TempSurface;
	ar::SceneFactory m_Factory;

	// Processors
	void ProcessAdd(EditorState& state);
	void ProcessSelect(EditorState& state);
	void ProcessDetach(EditorState& state);
	void ProcessAttach(EditorState& state);
	void ProcessPicking(EditorState& state);
	void ProcessGroupTransform(EditorState& state);
	void ProcessAddIntersection(EditorState& state);
	void ProcessUpdateVisibility(EditorState& state);
	void ProcessHeightmap(EditorState& state);

	// Cursor
	void PlaceCursor(ar::mat::Vec2 clickPosition, ViewportSize viewport, ar::mat::Vec3& cursorPosition);

	// Moving around
	void ProcessEntityGrab(EditorState& state);

	// Deletion
	void DeleteEntities(std::unordered_set<ar::Entity, ar::Entity::HashFunction>& entities);

	// Addition
	void CreateTempSurface(ar::SurfaceDesc desc, ar::mat::Vec3 position);
	void UpdateTempSurface(ar::SurfaceDesc& desc, ar::mat::Vec3 position);
	void AcceptTempSurface(EditorState& state);
	void RejectTempSurface();

	void AttachPointToCurves(ar::Entity point, std::vector<ar::Entity> curves);
	
	// Selection
	void SelectEntities(std::vector<ar::Entity> entities, bool add = false);
	void DeselectEntities(std::vector<ar::Entity> entities);
	void UpdateMeanPoint(EditorState& state);
	void BeginGroupTransform(EditorState& state);
	void EndGroupTransform(EditorState& state);

	// Detach
	void DetachPoint(ar::Entity child, ar::Entity parent);

	// Collapse
	bool CollapsePoints(std::vector<ar::Entity> points);
	ar::mat::Vec3 ComputeAveragePosition(ar::Entity p1, ar::Entity p2);
	void ReplaceChildRefs(ar::Entity oldRef, ar::Entity newRef);
	void ReplaceParents(ar::Entity oldRef, ar::Entity newRef);

	// Validate
	void ValidateGeometry(EditorState& state);
	void ValidateSelection(EditorState& state);
	void CorrectPointColors(EditorState& state);
	void ProcessIntCurveState(EditorState& state);
};