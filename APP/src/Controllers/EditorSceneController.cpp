#include "EditorSceneController.h"
#include "EditorConstants.h"

EditorSceneController::EditorSceneController(ar::Ref<ar::Scene> scene, ar::SceneRenderer& sceneRender)
	: m_Scene(scene), m_SceneRenderer(sceneRender),
	m_CameraController(std::make_shared<ar::CameraController>(
		EditorCameraConstants::FOV, 0.0f,
		EditorCameraConstants::NearPlane, EditorCameraConstants::FarPlane,
		EditorCameraConstants::ArcballRadius))
{ }

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
		PlaceCursor(state.ClickPosition, state.Viewport, state.CursorPosition);
		state.ShouldPlaceCursor = false;
	}
	if (state.ViewportResized)
	{
		m_CameraController->SetAspectRatio(state.Viewport.Width / state.Viewport.Height);
		m_SceneRenderer.OnResize({ state.Viewport.Width, state.Viewport.Height });
		state.ViewportResized = false;
	}

	// Validation
	if (geometryValidation)
		ValidateGeometry();
	if (selectionValidation)
		ValidateSelection(state);
}

ar::Entity EditorSceneController::AddPoint(ar::mat::Vec3 spawnPoint)
{
	auto entity = m_Scene->CreateEntity("Point");

	// Point tag component
	entity.AddComponent<ar::PointComponent>();

	// Transform component
	auto& trc = entity.AddComponent<ar::TransformComponent>();
	trc.Translation = spawnPoint;
	trc.IsRotationEnabled = false;
	trc.IsScaleEnabled = false;

	return entity;
}

void EditorSceneController::AddPointToCurves(ar::Entity point, std::vector<ar::Entity> curves)
{
	for (auto& curve : curves)
	{
		auto& pts = curve.GetComponent<ar::ControlPointsComponent>().Points;
		pts.push_back(point);
	}
}

void EditorSceneController::AddTorus(ar::mat::Vec3 spawnPoint, ar::TorusDesc desc)
{
	auto entity = m_Scene->CreateEntity("Torus");

	// Torus component
	auto& tc = entity.AddComponent<ar::TorusComponent>();
	tc.Description = desc;
	tc.Vertices = ar::TorusUtils::GenerateTorusVertices(desc);
	tc.Edges = ar::TorusUtils::GenerateTorusEdges(desc);

	// Transform component
	auto& trc = entity.AddComponent<ar::TransformComponent>();
	trc.Translation = spawnPoint;

	// Mesh (render) component
	auto& mc = entity.AddComponent<ar::MeshComponent>();

	// Primitive
	mc.RenderPrimitive = ar::Primitive::LineLoop;

	// VertexArray
	mc.VertexArray = ar::Scope<ar::VertexArray>(ar::VertexArray::Create());
	mc.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(tc.Vertices)));
	auto indexBuffers = ar::IndexBuffer::Create(tc.Edges);
	for (auto& ib : indexBuffers)
		mc.VertexArray->AddIndexBuffer(ib);

	// Shader
	mc.Shader = ar::ShaderLib::Get("Basic");
}

void EditorSceneController::AddChain(std::vector<ar::Entity> points)
{
	AR_ASSERT(points.size() > 1, "Too few points to create a chain.");
	auto entity = m_Scene->CreateEntity("Chain");
	entity.AddComponent<ar::ChainTagComponent>();
	entity.AddComponent<ar::ControlPointsComponent>(points);
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

void EditorSceneController::DetachFromChain(ar::Entity child, ar::Entity parent)
{
	auto& points = parent.GetComponent<ar::ControlPointsComponent>().Points;
	points.erase(std::remove(points.begin(), points.end(), child), points.end());
}

void EditorSceneController::ValidateGeometry()
{
	auto chains = m_Scene->m_Registry.view<ar::ChainTagComponent>();
	for (auto& chain : chains)
	{
		ar::Entity e = { chain, m_Scene.get() };
		if (!ar::CurveUtils::ValidateChain(e))
			m_Scene->DestroyEntity(e);
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
	for (auto& entity : state.SelectedObjects)
	{
		if (entity.HasComponent<ar::PointComponent>())
			state.SelectedPoints.push_back(entity);
		if (entity.HasAnyComponent<ar::ChainTagComponent>())
			state.SelectedCurves.push_back(entity);
	}
}

void EditorSceneController::ProcessAdd(EditorState& state)
{
	switch (state.AddObjectType)
	{
	case ar::ObjectType::POINT:
	{
		auto pt = AddPoint(state.CursorPosition);
		if (!state.SelectedCurves.empty())
			AddPointToCurves(pt, state.SelectedCurves);
		break;
	}
	case ar::ObjectType::TORUS:
	{
		AddTorus(state.CursorPosition, {});
		break;
	}
	case ar::ObjectType::CHAIN:
	{
		if (state.SelectedPoints.size() < 2)
		{
			state.ShowErrorModal = true;
			state.ErrorMessages.emplace_back("To create a chain, you need at least 2 points selected in the scene hierarchy!");
		}
		else
			AddChain(state.SelectedPoints);
		break;
	}
	case ar::ObjectType::NONE:
	default:
		AR_ASSERT(false, "Trying to create an object with uknown type");
	}

	state.ClearAddState();
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
			SelectEntities(state.SelectionCandidates, true);
			for (auto& e : state.SelectionCandidates)
				if (std::find(state.SelectedObjects.begin(), state.SelectedObjects.end(), e) == state.SelectedObjects.end())
				{
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
		if (pair.Parent.HasComponent<ar::ChainTagComponent>())
			DetachFromChain(pair.Child, pair.Parent);
	}
}

void EditorSceneController::ProcessAttach(EditorState& state)
{
	for (auto& pair : state.PairsToAttach)
	{
		auto& cp = pair.Parent.GetComponent<ar::ControlPointsComponent>().Points;
		// don't add points if they are already under the parent
		if (std::find(cp.begin(), cp.end(), pair.Child) == cp.end())
			cp.push_back(pair.Child);
	}
}

void EditorSceneController::ProcessPicking(EditorState& state)
{
	auto ids = m_SceneRenderer.ReadPixels(state.PickClickStart, state.PickClickEnd);
	
	auto entities = std::vector<ar::Entity>();
	for (auto& id : ids)
	{
		auto e = m_Scene->GetEntityByID(id);
		if (e) state.SelectionCandidates.push_back(e);
	}
	state.ShouldUpdateSelection = true;
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
	auto cursorPos = origin + direction * t;

	cursorPosition = cursorPos;
}

void EditorSceneController::DeleteEntities(std::vector<ar::Entity>& entities)
{
	for (auto& entity : entities)
	{
		if (entity.HasComponent<ar::PointComponent>())
		{
			// remove from all point composites
			auto view = m_Scene->m_Registry.view<ar::ControlPointsComponent>();
			for (auto [e, cp] : view.each())
			{
				cp.Points.erase(std::remove(cp.Points.begin(), cp.Points.end(), entity), cp.Points.end());
			}
		}
		m_Scene->DestroyEntity(entity);
	}
}
