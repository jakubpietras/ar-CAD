#pragma once
#include <vector>
#include "core/Scene/Entity.h"
#include "core/Scene/ObjectTypes.h"
#include "core/Utils/SurfaceUtils.h"
#include "core/Geometry/HoleDetector.h"
#include "core/Drawing/PaintSurface.h"
#include "core/Paths/HeightmapGenerator.h"
#include <filesystem>

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

	// -- Add surface
	bool ShouldShowSurfaceC0Modal = false;
	bool ShouldShowSurfaceC2Modal = false;
	bool ShouldShowGregoryModal = false;

	ar::SurfaceDesc NewSurfaceDesc{ {1, 1}, {4, 4} };
	bool NewSurfaceBegin = false;
	bool NewSurfaceDescChanged = false;						// recreate temporary object
	bool NewSurfaceAccepted = false;						// add real points
	bool NewSurfaceRejected = false;						// destroy temporary object
	void ClearAddState();


	// =========================== Delete ==========================
	std::unordered_set<ar::Entity, ar::Entity::HashFunction> ObjectsToDelete{};
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
	std::vector<ar::Entity> SelectedObjectsWithTransforms{};
	std::vector<ar::Entity> SelectedSurfacesC0{};
	std::vector<ar::Entity> SelectedIntersectableSurfaces{};
	std::vector<ar::Entity> SelectionCandidates{};
	std::optional<ar::Entity> SelectedIntersectionCurve = std::nullopt;
	std::vector<ar::Entity> SelectedIntersectionCurves{};
	ar::mat::Vec3 SelectedMeanPosition {0.f, 0.f, 0.f};
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
	ar::mat::Vec2 MousePrevPosViewport{ 0.0f, 0.0f };
	ar::mat::Vec2 MousePosViewport{ 0.f, 0.f };
	ar::mat::Vec2 MousePosChange{ 0.0f, 0.0f };
	ar::mat::Vec2 MousePosGlobal{ 0.f, 0.f };

	// =========================== Moving ==========================
	float MoveDx = 0.f, MoveDy = 0.f;
	bool MoveVertical = false;
	bool EntityGrabbed = false;

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

	// ============================= Group Transform ===========================
	std::unordered_map<uint32_t, ar::TransformComponent> OriginalTransforms{};
	ar::mat::Vec3 GroupTranslation{ 0.f, 0.f, 0.f };
	ar::mat::Vec3 GroupAnglesRPY{ 0.f, 0.f, 0.f };
	ar::mat::Vec3 GroupScale{ 1.f, 1.f, 1.f };
	bool UniformGroupScale = false;

	bool ShouldBeginGroupTransform = false;
	bool ShouldApplyGroupTransform = false;
	bool ShouldEndGroupTransform = false;
	void ClearGroupTransformState();

	// ============================= Serialization ===========================
	std::string Filepath;
	bool ShouldImport = false;
	bool ShouldExport = false;

	// ============================= Collapse ===========================
	// Collapse two points taken from SelectedPoints
	bool ShowCollapseModal = false;
	bool ShouldCollapsePoints = false;

	// ============================= Fill-in ===========================
	std::vector<ar::Hole> FillCandidates{};
	std::optional<ar::Hole> HoleToFill = std::nullopt;
	bool ShouldScanForHoles = false;
	bool HoleSelectionChanged = false;
	void ClearFillState();

	// ============================= Intersection =============================
	bool ShouldShowIntersectModal = false;
	bool ShouldComputeIntersection = false;
	bool ShouldDisplayParameterImage = false;
	bool ShouldUseCursorAssist = false;
	ar::Ref<ar::PaintSurface> ImageToDisplay = nullptr;
	float StepDistance = 0.01f;

	// ============================= Hide/Show =============================
	std::unordered_set<ar::Entity, ar::Entity::HashFunction> ObjectsToHide{};
	std::unordered_set<ar::Entity, ar::Entity::HashFunction> ObjectsToShow{};
	bool ShouldUpdateVisibility = false;

	// ============================= Debug =============================
	bool ShouldRunDebug = false;

	// ============================= Rough Milling =============================
	bool ShouldComputeHeightmap = false;
	ar::HeightmapGenerator::HeightmapDesc HMDescription{};
	std::vector<float> HeightmapData{};
	ar::Ref<ar::Texture> HeightmapImage = nullptr;
	bool ShouldGenerateFaceMillPaths = false;
	bool ShouldGenerateBaseMillPaths = false;
	bool ShouldGenerateOutlineMillPaths = false;

	// ============================= General milling ==========================
	std::optional<ar::Entity> BaseSurface = std::nullopt;	// only base surface
	std::vector<ar::Entity> OutlineSurfaces{};				// only those intersecting the base
	std::vector<ar::Entity> ModelSurfaces{};				// all surfaces without base

	// ============================= Rough Milling =============================
	std::filesystem::path GCodeRoot{};

	// ============================= Base Milling =============================
	bool ShowSelectedIntCurveNormals = false;
	bool ShouldComputeAllIntCurves = false;
	std::vector<ar::Entity> OutlineCurves{};
	std::optional<ar::Entity> FirstOutline = std::nullopt;
	bool ShouldResizeOutlineCurves = false;
	bool ShouldStitchOutlineCurves = false;

};