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

void EditorState::ClearSelectionState()
{
	SelectedObjects.clear();
	SelectedPoints.clear();
	SelectionCandidates.clear();
	ShouldUpdateSelection = false;
	SelectionChangeMode = SelectionMode::Replace;
}

void EditorState::ClearErrorState()
{
	ErrorMessages.clear();
	ShowErrorModal = false;
}
