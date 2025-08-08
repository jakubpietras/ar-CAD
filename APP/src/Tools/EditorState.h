#pragma once
#include <vector>
#include "core/Scene/Entity.h"

struct EditorState
{
	// =========================== Delete ==========================
	std::vector<ar::Entity> ObjectsToDelete{};
	bool ShouldDeleteObjects = false;
	bool ShowDeleteModal = false;
	void ClearDeleteState();
	
	// =========================== Rename ==========================
	ar::Entity ObjectToRename{ entt::null, nullptr };
	char RenameBuffer[128] = "";
	bool ShouldRenameObject = false;
	bool ShowRenameModal = false;
	void ClearRenameState();

	// =========================== Selection ==========================
	std::vector<ar::Entity> SelectedObjects{};
	std::vector<ar::Entity> SelectedPoints{};
	void ClearSelectionState();

	// ============================= Errors ===========================
	std::vector<std::string> ErrorMessages{};
	bool ShowErrorModal = false;
	void ClearErrorState();
};