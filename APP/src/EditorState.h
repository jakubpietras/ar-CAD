#pragma once
#include <vector>
#include "core/Scene/Entity.h"
#include "core/Scene/ObjectTypes.h"

struct EntityLink
{
	ar::Entity Parent;
	ar::Entity Child;
};

struct ViewportSize
{
	float Width = 0, Height = 0;
};

enum class SelectionMode
{
	Replace,
	Add,
	Remove
};

struct EditorState
{
	// =========================== Add ==========================
	ar::ObjectType AddObjectType = ar::ObjectType::NONE;
	bool ShouldAddObject = false;
	void ClearAddState();

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

	// =========================== Detach ==========================
	std::vector<EntityLink> PairsToDetach{};
	bool ShouldDetachPairs = false;
	bool ShowDetachModal = false;
	void ClearDetachState();

	// =========================== Attach ==========================
	std::vector<EntityLink> PairsToAttach{};
	bool ShouldAttachPairs = false;
	bool ShowAttachModal = false;
	void ClearAttachState();

	// =========================== Selection ==========================
	std::vector<ar::Entity> SelectedObjects{};
	std::vector<ar::Entity> SelectedPoints{};
	std::vector<ar::Entity> SelectedCurves{};
	std::vector<ar::Entity> SelectionCandidates{};
	SelectionMode SelectionChangeMode = SelectionMode::Replace;
	bool ShouldUpdateSelection = false;
	void ClearSelectionState();

	// =========================== Picking ==========================
	ar::mat::Vec2 PickClickStart{ 0.f, 0.f };
	ar::mat::Vec2 PickClickEnd{ 0.f, 0.f };
	ar::mat::Vec2 BoxStart{ 0.f, 0.f };
	SelectionMode PickingMode = SelectionMode::Replace;
	bool IsBoxPicking = false;
	bool ShouldProcessPicking = false;
	void ClearPickingState();

	// =========================== Picking ==========================
	ar::mat::Vec2 MousePosViewport{ 0.f, 0.f };
	ar::mat::Vec2 MousePosGlobal{ 0.f, 0.f };

	// =========================== Viewport ==========================
	ViewportSize Viewport { 0.f, 0.f };
	bool ViewportResized = false;

	// =========================== Cursor ==========================
	ar::mat::Vec3 CursorPosition{ 0.0f, 0.0f, 0.0f };
	ar::mat::Vec2 ClickPosition{ 0.0f, 0.0f };
	bool ShouldPlaceCursor = false;

	// ============================= Errors ===========================
	std::vector<std::string> ErrorMessages{};
	bool ShowErrorModal = false;
	void ClearErrorState();
};