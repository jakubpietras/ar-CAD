#include "EditorState.h"

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

void EditorState::ClearSelectionState()
{
	SelectedObjects.clear();
	SelectedPoints.clear();
}

void EditorState::ClearErrorState()
{
	ErrorMessages.clear();
	ShowErrorModal = false;
}
