#include "EditorState.h"

void EditorState::ClearAddState()
{
	AddObjectType = ar::ObjectType::NONE;
	ShouldAddObject = false;
}

void EditorState::ClearDeleteState()
{
	ObjectsToDelete.clear();
	ShouldDeleteObjects = false;
	ShowDeleteModal = false;
}

void EditorState::ClearRenameState()
{
	ObjectToRename = { entt::null, nullptr };
	RenameBuffer[0] = '\0';
	ShouldRenameObject = false;
	ShowRenameModal = false;
}

void EditorState::ClearDetachState()
{
	PairsToDetach.clear();
	ShouldDetachPairs = false;
	ShowDetachModal = false;
}

void EditorState::ClearAttachState()
{
	PairsToAttach.clear();
	ShouldAttachPairs = false;
	ShowAttachModal = false;
}

void EditorState::ClearSelectionState()
{
	SelectedObjects.clear();
	SelectedPoints.clear();
	SelectionCandidates.clear();
	ShouldUpdateSelection = false;
	SelectionChangeMode = SelectionMode::Replace;
}

void EditorState::ClearPickingState()
{
	PickClickStart = { 0.f, 0.f };
	PickClickEnd = { 0.f, 0.f };
	ShouldProcessPicking = false;
}

void EditorState::ClearErrorState()
{
	ErrorMessages.clear();
	ShowErrorModal = false;
}

void EditorState::ClearGroupTransformState()
{
	GroupTranslation = { 0.f, 0.f, 0.f };
	GroupPreviousTranslation = { 0.f, 0.f, 0.f };

	GroupRotation = { 0.f, 0.f, 0.f };
	GroupPreviousRotation = { 0.f, 0.f, 0.f };

	GroupScale = { 0.f, 0.f, 0.f };
	GroupPreviousScale = { 0.f, 0.f, 0.f };
}
