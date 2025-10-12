#pragma once
#include <vector>
#include <string>
#include "Milling/MaterialDesc.h"
#include "Milling/CutterType.h"
#include <filesystem>

namespace fs = std::filesystem;

struct ViewportSize
{
	float Width = 0, Height = 0;
};

class SimState
{
public:
	// ============ VIEWPORT ==============
	ViewportSize	Viewport{ 0.f, 0.f };
	bool			ViewportResized = false;

	// ============ ERRORS ================
	std::vector<std::string> ErrorMessages;
	bool			ShowErrorModal = false;
	inline void ClearErrorState()
	{
		ErrorMessages.clear();
		ShowErrorModal = false;
	}

	// ============ LOADING ===============
	fs::path		Filepath;
	bool			ShouldImport = false;
	inline void ClearImportState()
	{
		ShouldImport = false;
	}

	// ============ MILLING ===============
	MaterialDesc	Material{};
	float			SimulationSpeed = 10.0;
	bool			IsSimulationRun = false;
	bool			ShouldMillInstant = false;
	float			CutterSize = 1.0;
	float			CutterHeight = 4.0;
	CutterType		CutterType = CutterType::FLAT;
	bool			ShouldShowPaths = false;
};