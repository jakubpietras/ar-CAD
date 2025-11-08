#include "EditorSceneController.h"
#include "EditorConstants.h"
#include "core/Renderer/Shader.h"
#include "core/Utils/GeneralUtils.h"
#include "core/Serialization/SceneImporter.h"
#include "core/Serialization/SceneExporter.h"
#include <algorithm>
#include "core/Geometry/HoleDetector.h"
#include "core/Geometry/GregoryFill.h"
#include "core/Scene/Components.h"
#include "core/Intersections/Intersection.h"
#include "core/Scene/DebugRenderer.h"
#include "core/Utils/CurveUtils.h"
#include "core/Tests/tests.h"
#include "core/Paths/HeightmapGenerator.h"
#include "core/Paths/ToolPath.h"

EditorSceneController::EditorSceneController(ar::Ref<ar::Scene> scene, ar::SceneRenderer& sceneRender)
	: m_Scene(scene), m_SceneRenderer(sceneRender),
	m_CameraController(std::make_shared<ar::CameraController>(
		EditorCameraConstants::FOV, 0.0f,
		EditorCameraConstants::NearPlane, EditorCameraConstants::FarPlane,
		EditorCameraConstants::ArcballRadius)),
	m_Factory(scene)
{
}

void EditorSceneController::ProcessStateChanges(EditorState& state)
{
	bool geometryValidation = false,
		selectionValidation = false;

	if (state.ShouldProcessPicking)
	{
		ProcessPicking(state);
		state.ClearPickingState();
	}
	if (state.ShouldAddObject)
	{
		ProcessAdd(state);
		state.ClearAddState();
	}
	if (state.ShouldCollapsePoints)
	{
		if (CollapsePoints(state.SelectedPoints))
		{
			state.ObjectsToDelete.insert(state.SelectedPoints.begin(), state.SelectedPoints.end());
			state.ShouldDeleteObjects = true;
		}
		state.ShouldCollapsePoints = false;
	}
	if (state.ShouldDeleteObjects)
	{
		DeleteEntities(state.ObjectsToDelete);
		state.ClearDeleteState();
		geometryValidation = true;
		selectionValidation = true;
	}
	if (state.ShouldUpdateSelection)
	{
		ProcessSelect(state);
		selectionValidation = true;
	}
	if (state.ShouldRenameObject)
	{
		state.ObjectToRename.SetName(state.RenameBuffer);
		state.ClearRenameState();
	}
	if (state.ShouldDetachPairs)
	{
		ProcessDetach(state);
		state.ClearDetachState();
		geometryValidation = true;
		selectionValidation = true;
	}
	if (state.ShouldAttachPairs)
	{
		ProcessAttach(state);
		state.ClearAttachState();
	}
	if (state.ShouldPlaceCursor)
	{
		PlaceCursor(state.MousePosViewport, state.Viewport, state.CursorPosition);
		state.ShouldPlaceCursor = false;
	}
	if (state.ViewportResized)
	{
		m_CameraController->SetAspectRatio(state.Viewport.Width / state.Viewport.Height);
		m_SceneRenderer.OnResize({ state.Viewport.Width, state.Viewport.Height });
		state.ViewportResized = false;
	}

	// Group Transformss
	if (state.ShouldBeginGroupTransform)
	{
		BeginGroupTransform(state);
		state.ShouldBeginGroupTransform = false;
	}
	if (state.ShouldApplyGroupTransform)
	{
		ProcessGroupTransform(state);
		state.ShouldApplyGroupTransform = false;
	}
	if (state.ShouldEndGroupTransform)
	{
		EndGroupTransform(state);
		state.ShouldEndGroupTransform = false;
	}

	// Add surfaces
	if (state.NewSurfaceBegin)
	{
		CreateTempSurface(state.NewSurfaceDesc, state.CursorPosition);
		state.NewSurfaceBegin = false;
	}
	if (state.NewSurfaceDescChanged)
	{
		UpdateTempSurface(state.NewSurfaceDesc, state.CursorPosition);
		state.NewSurfaceDescChanged = false;
	}
	if (state.NewSurfaceAccepted)
	{
		AcceptTempSurface(state);
		state.NewSurfaceDesc = { {1, 1}, {4, 4} };
		state.NewSurfaceAccepted = false;
	}
	if (state.NewSurfaceRejected)
	{
		RejectTempSurface();
		state.NewSurfaceDesc = { {1, 1}, {4, 4} };
		state.NewSurfaceRejected = false;
	}

	if (state.ShouldImport)
	{
		ar::SceneImporter::Import(state.Filepath, m_Factory);
		state.ShouldImport = false;
		state.Filepath = "";
	}
	if (state.ShouldExport)
	{
		ar::SceneExporter::Export(state.Filepath, m_Scene);
		state.ShouldExport = false;
		state.Filepath = "";
	}

	if (state.ShouldScanForHoles)
	{
		state.FillCandidates = ar::HoleDetector::Detect(state.SelectedSurfacesC0);
		state.ShouldScanForHoles = false;
	}
	if (state.HoleSelectionChanged)
	{
		CorrectPointColors(state);
		state.HoleSelectionChanged = false;
	}
	if (state.ShouldComputeIntersection)
	{
		ProcessAddIntersection(state);
		state.StepDistance = 0.01f;
		state.ShouldComputeIntersection = false;
	}
	if (state.ShouldUpdateVisibility)
	{
		ProcessUpdateVisibility(state);
		state.ShouldUpdateVisibility = false;
	}
	if (state.ShouldRunDebug)
	{
		//ar::Tests::TestLineSearchSuite();
		ar::ToolPath tp({ 0.f, 0.f, 6.f }, ar::ToolType::K16);
		tp.ConvertToGCode(1, state.GCodeRoot);
		state.ShouldRunDebug = false;
	}
	if (state.EntityGrabbed)
	{
		ProcessEntityGrab(state);
	}

	if (state.ShouldComputeHeightmap)
	{
		ProcessHeightmap(state);
		state.ShouldComputeHeightmap = false;
	}

	// Validation
	if (geometryValidation)
		ValidateGeometry(state);
	if (selectionValidation)
		ValidateSelection(state);
	ProcessIntCurveState(state);
}

void EditorSceneController::AttachPointToCurves(ar::Entity point, std::vector<ar::Entity> curves)
{
	for (auto& curve : curves)
	{
		auto& pts = curve.GetComponent<ar::ControlPointsComponent>().Points;
		pts.push_back(point);
		point.GetComponent<ar::PointComponent>().Parents.push_back(curve);
	}
}

void EditorSceneController::SelectEntities(std::vector<ar::Entity> entities, bool add /*= false*/)
{
	for (auto& entity : entities)
	{
		entity.AddComponent<ar::SelectedTagComponent>();
	}
}


void EditorSceneController::DeselectEntities(std::vector<ar::Entity> entities)
{
	for (auto& entity : entities)
	{
		entity.RemoveComponent<ar::SelectedTagComponent>();
	}
}

void EditorSceneController::UpdateMeanPoint(EditorState& state)
{
	// compute mean position of the selected objects
	if (state.SelectedObjectsWithTransforms.size() > 1)
	{
		ar::mat::Vec3 sum{ 0.f, 0.f, 0.f };
		uint32_t count = 0;
		for (auto& object : state.SelectedObjectsWithTransforms)
		{
			auto& transform = object.GetComponent<ar::TransformComponent>();
			sum += transform.Translation;
			count++;
		}
		state.SelectedMeanPosition = sum / static_cast<float>(count);
	}
	else
		state.SelectedMeanPosition = { 0.f, 0.f, 0.f };
}

void EditorSceneController::BeginGroupTransform(EditorState& state)
{
	state.OriginalTransforms.clear();
	for (auto& entity : state.SelectedObjects)
	{
		if (entity.HasComponent<ar::TransformComponent>())
		{
			auto& t = entity.GetComponent<ar::TransformComponent>();
			t.GroupTransformation = true;
			ar::TransformComponent transformCopy = t;
			state.OriginalTransforms.emplace(entity.GetID(), transformCopy);
		}
	}
}

void EditorSceneController::EndGroupTransform(EditorState& state)
{
	state.ClearGroupTransformState();
	for (auto& entity : state.SelectedObjects)
	{
		if (entity.HasComponent<ar::TransformComponent>())
		{
			auto& t = entity.GetComponent<ar::TransformComponent>();
			t.GroupTransformation = false;
		}
	}
	state.OriginalTransforms.clear();
	UpdateMeanPoint(state);
}

void EditorSceneController::DetachPoint(ar::Entity child, ar::Entity parent)
{
	auto& points = parent.GetComponent<ar::ControlPointsComponent>().Points;
	points.erase(std::remove(points.begin(), points.end(), child), points.end());

	parent.GetComponent<ar::MeshComponent>().DirtyFlag = true;
}

bool EditorSceneController::CollapsePoints(std::vector<ar::Entity> points)
{
	if (points.size() != 2)
		return false;

	auto position = ComputeAveragePosition(points[0], points[1]);
	auto collapsedPoint = m_Factory.CreatePoint(position, std::nullopt, "Collapsed Point");

	ReplaceChildRefs(points[0], collapsedPoint);
	ReplaceChildRefs(points[1], collapsedPoint);

	ReplaceParents(points[0], collapsedPoint);
	ReplaceParents(points[1], collapsedPoint);

	return true;
}

ar::mat::Vec3 EditorSceneController::ComputeAveragePosition(ar::Entity p1, ar::Entity p2)
{
	auto t1 = p1.GetComponent<ar::TransformComponent>().Translation;
	auto t2 = p2.GetComponent<ar::TransformComponent>().Translation;
	return (t1 + t2) / 2;
}

void EditorSceneController::ReplaceChildRefs(ar::Entity oldRef, ar::Entity newRef)
{
	AR_ASSERT(oldRef.HasComponent<ar::PointComponent>(), "Replacing refs works on points only!");
	AR_ASSERT(newRef.HasComponent<ar::PointComponent>(), "Replacing refs works on points only!");

	auto& parents = oldRef.GetComponent<ar::PointComponent>().Parents;
	for (auto& parent : parents)
	{
		auto& cp = parent.GetComponent<ar::ControlPointsComponent>().Points;
		std::replace_if(cp.begin(), cp.end(),
			[&oldRef](ar::Entity e) { return e.GetID() == oldRef.GetID(); },
			newRef);
	}
}

void EditorSceneController::ReplaceParents(ar::Entity oldRef, ar::Entity newRef)
{
	AR_ASSERT(oldRef.HasComponent<ar::PointComponent>(), "Replacing refs works on points only!");
	AR_ASSERT(newRef.HasComponent<ar::PointComponent>(), "Replacing refs works on points only!");
	auto& parents = oldRef.GetComponent<ar::PointComponent>().Parents;
	auto& newParents = newRef.GetComponent<ar::PointComponent>().Parents;

	for (auto& parent : parents)
	{
		if (std::find(newParents.begin(), newParents.end(), parent) == newParents.end())
			newParents.push_back(parent);
	}
}

void EditorSceneController::ValidateGeometry(EditorState& state)
{
	// 1. Remove invalid curves from points
	auto view = m_Scene->m_Registry.view<ar::PointComponent>(entt::exclude<ar::VirtualTagComponent>);
	for (auto [entity, pc] : view.each())
	{
		auto point = ar::Entity(entity, m_Scene.get());
		pc.Parents.erase(std::remove_if(pc.Parents.begin(), pc.Parents.end(),
			[&](ar::Entity parent) 
			{
				if (!parent.IsValid())
					return true;

				// Check if parent still references this point
				if (parent.HasComponent<ar::ControlPointsComponent>())
				{
					auto& points = parent.GetComponent<ar::ControlPointsComponent>().Points;
					bool parentStillReferencesPoint = std::find(points.begin(), points.end(), point) != points.end();

					if (!parentStillReferencesPoint)
					{
						parent.GetComponent<ar::MeshComponent>().DirtyFlag = true;
						return true; 
					}
				}
				return false;
			}), pc.Parents.end());
	}

	// 2. Remove invalid points from curves
	auto cpView = m_Scene->m_Registry.view<ar::ControlPointsComponent>();
	for (auto [entity, cp] : cpView.each())
	{
		auto pointContainer = ar::Entity(entity, m_Scene.get());
		size_t originalSize = cp.Points.size();
		cp.Points.erase(std::remove_if(cp.Points.begin(), cp.Points.end(),
			[](ar::Entity point) { return !point.IsValid(); }), cp.Points.end());

		if (cp.Points.size() != originalSize)
		{
			pointContainer.GetComponent<ar::MeshComponent>().DirtyFlag = true;
		}
	}

	// 3. Validate and destroy invalid curves (check ALL curves, not just selected)
	std::vector<ar::Entity> curvesToDestroy;
	for (auto [entity, cp] : cpView.each())
	{
		ar::Entity curve(entity, m_Scene.get());
		if (!ar::CurveUtils::ValidateCurve(curve))
		{
			// Clean up parent references before destroying
			for (auto& point : cp.Points)
			{
				if (point.IsValid())
				{
					auto& parents = point.GetComponent<ar::PointComponent>().Parents;
					parents.erase(std::remove(parents.begin(), parents.end(), curve), parents.end());
				}
			}
			curvesToDestroy.push_back(curve);
		}
	}

	// 4. Destroy invalid curves
	for (auto& curve : curvesToDestroy)
	{
		m_Scene->DestroyEntity(curve);
	}

	// 5. Remove invalid intersection curves from trimming components
	auto trimView = m_Scene->m_Registry.view<ar::TrimmingComponent>();
	for (auto [entity, tc] : trimView.each())
	{
		for (auto& intCurve : tc.IntersectionCurves)
		{
			if (!intCurve.IsValid())
			{
				tc.ShouldTrimSurface = false;
				tc.IntersectionCurves.erase(std::remove(tc.IntersectionCurves.begin(),
					tc.IntersectionCurves.end(), intCurve), tc.IntersectionCurves.end());
				tc.TrimTexture = nullptr;
			}
		}
	}
}

void EditorSceneController::ValidateSelection(EditorState& state)
{
	state.SelectedObjects.erase(
		std::remove_if(state.SelectedObjects.begin(), state.SelectedObjects.end(),
			[&](ar::Entity e) { return !e.IsValid(); }),
		state.SelectedObjects.end()
	);
	state.SelectedPoints.clear();
	state.SelectedCurves.clear();
	state.SelectedObjectsWithTransforms.clear();
	state.SelectedSurfacesC0.clear();
	state.SelectedIntersectableSurfaces.clear();

	for (auto& entity : state.SelectedObjects)
	{
		if (entity.HasComponent<ar::PointComponent>() 
			&& !entity.HasComponent<ar::VirtualTagComponent>())
			state.SelectedPoints.push_back(entity);
		if (entity.HasAnyComponent<ar::ChainComponent, ar::CurveC0Component, ar::CurveC2Component, ar::InterpolatedC2Component>())
			state.SelectedCurves.push_back(entity);
		if (entity.HasComponent<ar::TransformComponent>())
			state.SelectedObjectsWithTransforms.push_back(entity);
		if (entity.HasComponent<ar::SurfaceComponent>())
		{
			auto type = entity.GetComponent<ar::SurfaceComponent>().Description.Type;
			if (type == ar::SurfaceType::RECTANGLEC0 || type == ar::SurfaceType::CYLINDERC0)
				state.SelectedSurfacesC0.push_back(entity);
		}
		if (entity.HasAnyComponent<ar::SurfaceComponent, ar::TorusComponent>())
			state.SelectedIntersectableSurfaces.push_back(entity);
	}

	if (!state.SelectedObjects.empty())
	{
		if (state.SelectedObjects.back().HasComponent<ar::IntersectCurveComponent>())
			state.SelectedIntersectionCurve = state.SelectedObjects.back();
		else
			state.SelectedIntersectionCurve = std::nullopt;
	}
	// middle point
	UpdateMeanPoint(state);
}

void EditorSceneController::CorrectPointColors(EditorState& state)
{
	auto view = m_Scene->m_Registry.view<ar::PointComponent>();
	for (const auto& [e, pt] : view.each())
	{
		pt.ShouldUseTempColor = false;
	}
	
	if (state.HoleToFill)
	{
		auto holePoints = state.HoleToFill->Endpoints;
		for (const auto& point : holePoints)
		{
			auto& pt = m_Scene->GetEntityByID(point).GetComponent<ar::PointComponent>();
			pt.TempColor = ar::Renderer::HOLE_COLOR;
			pt.ShouldUseTempColor = true;
		}
	}
}

void EditorSceneController::ProcessIntCurveState(EditorState& state)
{
	if (state.SelectedIntersectionCurve)
	{
		if (!state.SelectedIntersectionCurve->IsValid())
		{
			state.SelectedIntersectionCurve = std::nullopt;
			return;
		}
		auto& intcurve = (*state.SelectedIntersectionCurve).GetComponent<ar::IntersectCurveComponent>();
		if (intcurve.ShowImageP)
		{
			state.ImageToDisplay = intcurve.ImageP;
			if (intcurve.ShowImage)
				state.ShouldDisplayParameterImage = true;
		}
		else if (intcurve.ShowImageQ)
		{
			state.ImageToDisplay = intcurve.ImageQ;
			if (intcurve.ShowImage)
				state.ShouldDisplayParameterImage = true;
		}

		if (intcurve.ConvertToSpline)
		{
			ar::CurveUtils::ConvertIntersectCurve(*state.SelectedIntersectionCurve, m_Factory);
			state.SelectedIntersectionCurve = std::nullopt;
		}
	}
}

void EditorSceneController::ProcessAdd(EditorState& state)
{
	switch (state.AddObjectType)
	{
	case ar::ObjectType::POINT:
	{
		//auto pt = AddPoint(state.CursorPosition);

		auto pt = m_Factory.CreatePoint(state.CursorPosition, std::nullopt, "Point");
		if (!state.SelectedCurves.empty())
			AttachPointToCurves(pt, state.SelectedCurves);
		break;
	}
	case ar::ObjectType::TORUS:
	{
		m_Factory.CreateTorus(state.CursorPosition, {}, std::nullopt, "Torus");
		break;
	}
	case ar::ObjectType::CHAIN:
	{
		if (state.SelectedPoints.size() < 2)
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("To create a chain, you need at least 2 points selected!");
		}
		else
			m_Factory.CreateChain(state.SelectedPoints, std::nullopt, "Chain");
		break;
	}
	case ar::ObjectType::BEZIERC0:
	{
		if (state.SelectedPoints.size() < 2)
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("To create a Bezier curve, you need at least 2 points selected!");
		}
		else
			m_Factory.CreateCurveC0(state.SelectedPoints, std::nullopt, "CurveC0");
		break;
	}
	case ar::ObjectType::BEZIERC2:
	{
		if (state.SelectedPoints.size() < 2)
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("To create a B-spline curve, you need at least 2 points selected!");
		}
		else
			m_Factory.CreateCurveC2(state.SelectedPoints, std::nullopt, "CurveC2");
		break;
	}
	case ar::ObjectType::INTERPOLATEDC2:
	{
		if (state.SelectedPoints.size() < 2)
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("To create an interpolated C2 curve, you need at least 2 points selected!");
		}
		else
			m_Factory.CreateInterpolatedC2(state.SelectedPoints, std::nullopt, "InterpolatedC2");
		break;
	}
	case ar::ObjectType::GREGORY:
	{
		if (!state.HoleToFill.has_value())
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("Cannot create a Gregory patch without a hole to fill in");
		}
		else
			m_Factory.CreateGregoryPatch(*state.HoleToFill, std::nullopt, "Gregory Patch");
		state.ClearFillState();
	}
	case ar::ObjectType::NONE:
	default:
		AR_ASSERT(false, "Trying to create an object with uknown type");
	}

}

void EditorSceneController::ProcessSelect(EditorState& state)
{
	switch (state.SelectionChangeMode)
	{
		case SelectionMode::Replace:
		{			
			DeselectEntities(state.SelectedObjects);
			SelectEntities(state.SelectionCandidates);
			state.SelectedObjects = state.SelectionCandidates;
			break;
		}
		case SelectionMode::Add:
		{
			for (auto& e : state.SelectionCandidates)
				if (std::find(state.SelectedObjects.begin(), state.SelectedObjects.end(), e) == state.SelectedObjects.end())
				{
					SelectEntities({e}, true);
					state.SelectedObjects.push_back(e);
				}
			break;
		}
		case SelectionMode::Remove:
		{
			DeselectEntities(state.SelectionCandidates);
			for (auto& e : state.SelectionCandidates)
			{
				state.SelectedObjects.erase(
					std::remove(state.SelectedObjects.begin(), state.SelectedObjects.end(), e),
					state.SelectedObjects.end()
				);
			}
			break;
		}
	}
	// clean up temporary buffers and flags
	state.SelectionCandidates.clear();
	state.ShouldUpdateSelection = false;
}

void EditorSceneController::ProcessDetach(EditorState& state)
{
	for (auto& pair : state.PairsToDetach)
	{
		DetachPoint(pair.Child, pair.Parent);
	}
}

void EditorSceneController::ProcessAttach(EditorState& state)
{
	for (auto& pair : state.PairsToAttach)
	{
		auto& cp = pair.Parent.GetComponent<ar::ControlPointsComponent>().Points;
		cp.push_back(pair.Child);
		pair.Child.GetComponent<ar::PointComponent>().Parents.push_back(pair.Parent);
		pair.Parent.GetComponent<ar::MeshComponent>().DirtyFlag = true;
	}
}

void EditorSceneController::ProcessPicking(EditorState& state)
{
	auto ids = m_SceneRenderer.ReadPixels(state.PickClickStart, state.PickClickEnd);
	
	auto entities = std::vector<ar::Entity>();
	
	state.SelectionChangeMode = state.PickingMode;
	for (auto& id : ids)
	{
		auto e = m_Scene->GetEntityByID(id);
		if (std::find(state.SelectionCandidates.begin(), state.SelectionCandidates.end(), e) == state.SelectionCandidates.end())
			state.SelectionCandidates.push_back(e);
	}
	state.ShouldUpdateSelection = true;
}

void EditorSceneController::ProcessGroupTransform(EditorState& state)
{
	for (auto& object : state.SelectedObjectsWithTransforms)
	{
		auto& originalTransform = state.OriginalTransforms.at(object.GetID());
		auto& transform = object.GetComponent<ar::TransformComponent>();


		transform.PreviousTranslation = originalTransform.Translation;
		transform.PreviousAnglesRPY = originalTransform.AnglesRPY;
		transform.PreviousScale = originalTransform.Scale;

		transform.Translation = originalTransform.Translation + state.GroupTranslation;

		transform.AnglesRPY = originalTransform.AnglesRPY + state.GroupAnglesRPY;
		transform.AnglesRPY.x = std::fmod(transform.AnglesRPY.x + 180.0f, 360.0f) - 180.0f;
		transform.AnglesRPY.y = std::fmod(transform.AnglesRPY.y + 180.0f, 360.0f) - 180.0f;
		transform.AnglesRPY.z = std::fmod(transform.AnglesRPY.z + 180.0f, 360.0f) - 180.0f;

		transform.Scale = originalTransform.Scale * state.GroupScale;

		transform.GroupTransformation = true;
		transform.DirtyFlag = true;
	}
}

void EditorSceneController::ProcessAddIntersection(EditorState& state)
{
	auto& objs = state.SelectedIntersectableSurfaces;
	ar::ICData curve;
	std::vector<ar::mat::Vec3> points; 
	std::vector<ar::mat::Vec4> params;

	if (objs.size() == 1)
	{
		curve = ar::Intersection::IntersectionCurve(objs[0], objs[0], state.StepDistance, ar::mat::Vec3d(state.CursorPosition), state.ShouldUseCursorAssist);
		points = ar::GeneralUtils::VecDoubleToFloat(curve.Points);
		params = ar::GeneralUtils::VecDoubleToFloat(curve.Params);
		if (!curve.Points.empty() && !curve.Params.empty())
		{
			m_Factory.CreateIntersectionCurve(points, params, objs[0], std::nullopt,
				std::nullopt, "Self-intersection Curve");
		}
		else
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("No self-intersection detected.");
		}

	}
	else if (objs.size() == 2)
	{
		curve = ar::Intersection::IntersectionCurve(objs[0], objs[1], state.StepDistance, ar::mat::Vec3d(state.CursorPosition), state.ShouldUseCursorAssist);
		points = ar::GeneralUtils::VecDoubleToFloat(curve.Points);
		params = ar::GeneralUtils::VecDoubleToFloat(curve.Params);
		if (!curve.Points.empty() && !curve.Params.empty())
		{
			auto ic = m_Factory.CreateIntersectionCurve(points, params, objs[0], objs[1],
				std::nullopt, "Intersection Curve");
		}
		else
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("No intersection detected.");
		}
	}
}

void EditorSceneController::ProcessUpdateVisibility(EditorState& state)
{
	// hide
	for (const ar::Entity& obj : state.ObjectsToHide) 
	{
		ar::Entity mutableObj = obj;
		if (!mutableObj.HasComponent<ar::HiddenTagComponent>())
			mutableObj.AddComponent<ar::HiddenTagComponent>();
	}
	// show
	for (const ar::Entity& obj : state.ObjectsToShow)
	{
		ar::Entity mutableObj = obj;
		if (mutableObj.HasComponent<ar::HiddenTagComponent>())
			mutableObj.RemoveComponent<ar::HiddenTagComponent>();
	}

	state.ObjectsToHide.clear();
	state.ObjectsToShow.clear();
}

void EditorSceneController::ProcessHeightmap(EditorState& state)
{
	if (state.SelectedIntersectableSurfaces.empty())
		return;
	if (state.HMDescription.SamplesX == 0 || state.HMDescription.SamplesY == 0)
		return;
	state.HeightmapData = ar::HeightmapGenerator::Generate(state.HMDescription, state.SelectedIntersectableSurfaces);
	ar::TextureDesc desc;
	desc.Width = state.HMDescription.SamplesX;
	desc.Height = state.HMDescription.SamplesY;
	desc.Format = ar::TextureFormat::R32F;
	auto tex = ar::Texture::Create(desc);
	tex->UpdateData(state.HeightmapData.data());
	state.HeightmapImage = ar::Ref<ar::Texture>(tex);
}

void EditorSceneController::PlaceCursor(ar::mat::Vec2 clickPosition, ViewportSize viewport, ar::mat::Vec3& cursorPosition)
{
	float xPos = clickPosition.x;
	float yPos = clickPosition.y;

	// convert to NDC
	float xNDC = (2.0f * xPos) / viewport.Width - 1.0f;
	float yNDC = 1.0f - (2.0f * yPos) / viewport.Height;
	auto rayClip = ar::mat::Vec4(xNDC, yNDC, -1.0f, 1.0f);

	// convert to camera space
	auto rayCamera = m_CameraController->GetCamera()->GetInvProjection() * rayClip;
	rayCamera = ar::mat::Vec4(rayCamera.x, rayCamera.y, -1.0f, 0.0f);

	// convert to world space
	auto direction = ar::mat::Normalize(ar::mat::ToVec3(m_CameraController->GetCamera()->GetInvView() * rayCamera));

	ar::mat::Vec3 origin = ar::mat::ToVec3(m_CameraController->GetPosition());
	ar::mat::Vec3 planeNormal = ar::mat::ToVec3(m_CameraController->GetCamera()->GetForward());
	float t = -((ar::mat::Dot(origin, planeNormal)) / ar::mat::Dot(direction, planeNormal));
	auto cursorPos = -m_CameraController->GetOffset() + origin + direction * t;

	cursorPosition = cursorPos;
}

void EditorSceneController::ProcessEntityGrab(EditorState& state)
{
	const float sensitivity = 0.01;
	if (state.SelectedObjects.empty())
		return;
	auto& entities = state.SelectedObjects;
	for (auto& entity : entities)
	{
		if (!entity.HasComponent<ar::PointComponent>())
			return;
		auto& t = entity.GetComponent<ar::TransformComponent>();
		if (state.MoveVertical)
		{
			t.Translation.y += state.MousePosChange.y * sensitivity;
		}
		else
		{
			t.Translation.x -= state.MousePosChange.x * sensitivity;
			t.Translation.z += state.MousePosChange.y * sensitivity;
		}
		t.DirtyFlag = true;
	}
}

void EditorSceneController::DeleteEntities(std::unordered_set<ar::Entity, ar::Entity::HashFunction>& entities)
{
	for (auto& entity : entities)
	{
		m_Scene->DestroyEntity(entity);
	}
}

void EditorSceneController::CreateTempSurface(ar::SurfaceDesc desc, ar::mat::Vec3 position)
{
	std::vector<ar::mat::Vec3> positions;
	std::vector<uint32_t> indices;
	ar::Ref<ar::Shader> shader, pickingShader;

	m_TempSurface = m_Scene->CreateEntity(std::nullopt, "Surface");
	m_TempSurface.AddComponent<ar::VirtualTagComponent>();
	positions = ar::SurfaceUtils::GenerateSurfaceData(desc, position);
	indices = ar::SurfaceUtils::GenerateSurfaceIndices(desc);

	m_TempSurface.AddComponent<ar::SurfaceComponent>();
	if (desc.Type == ar::SurfaceType::RECTANGLEC0 || desc.Type == ar::SurfaceType::CYLINDERC0)
	{
		shader = ar::ShaderLib::Get("SurfaceC0");
		pickingShader = ar::ShaderLib::Get("SurfaceC0Picking");
	}
	else
	{
		shader = ar::ShaderLib::Get("SurfaceC2");
		pickingShader = ar::ShaderLib::Get("SurfaceC2Picking");
	}

	auto& mesh = m_TempSurface.AddComponent<ar::MeshComponent>();
	mesh.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
	mesh.VertexArray->AddVertexBuffer(
		ar::Ref<ar::VertexBuffer>(
			ar::VertexBuffer::Create(
			ar::GeneralUtils::GetVertexData(positions, m_TempSurface.GetID())
		))
	);
	mesh.VertexArray->AddIndexBuffer(
		ar::Ref<ar::IndexBuffer>(
			ar::IndexBuffer::Create(indices)
		));
	mesh.Shader = shader;
	mesh.PickingShader = pickingShader;
	mesh.RenderPrimitive = ar::Primitive::Patch;
	mesh.TessellationPatchSize = 16;
}

void EditorSceneController::UpdateTempSurface(ar::SurfaceDesc& desc, ar::mat::Vec3 position)
{
	std::vector<ar::mat::Vec3> positions;
	std::vector<uint32_t> indices;
	switch (desc.Type)
	{
		case ar::SurfaceType::RECTANGLEC0:
		{
			desc.Size = { 3 * desc.Segments.u + 1, 3 * desc.Segments.v + 1 };
			break;
		}
		case ar::SurfaceType::CYLINDERC0:
		{
			desc.Size = { 3 * desc.Segments.u, 3 * desc.Segments.v + 1 };
			break;
		}
		case ar::SurfaceType::RECTANGLEC2:
		{
			desc.Size = { desc.Segments.u + 3, desc.Segments.v + 3 };
			break;
		}
		case ar::SurfaceType::CYLINDERC2:
		{
			desc.Size = { desc.Segments.u, desc.Segments.v + 3 };
			break;
		}
	}

	positions = ar::SurfaceUtils::GenerateSurfaceData(desc, position);
	indices = ar::SurfaceUtils::GenerateSurfaceIndices(desc);

	auto& mesh = m_TempSurface.GetComponent<ar::MeshComponent>();
	mesh.VertexArray->ClearBuffers();
	mesh.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(positions, m_TempSurface.GetID()))));
	mesh.VertexArray->AddIndexBuffer(
		ar::Ref<ar::IndexBuffer>(
			ar::IndexBuffer::Create(indices)
		));
}

void EditorSceneController::AcceptTempSurface(EditorState& state)
{
	m_TempSurface.RemoveComponent<ar::VirtualTagComponent>();
	m_Factory.CreateSurface(state.CursorPosition, state.NewSurfaceDesc, m_TempSurface);
	m_TempSurface = ar::Entity(entt::null, nullptr);
}

void EditorSceneController::RejectTempSurface()
{
	m_Scene->DestroyEntity(m_TempSurface);
	m_TempSurface = ar::Entity(entt::null, nullptr);
}
