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
	bool			ShouldReset = false;
	float			SimulationSpeed = 10.0;
	bool			ShouldMillInstant = false;
	float			CutterSize = 1.0;
	float			CutterHeight = 4.0;
	CutterType		CutterType = CutterType::FLAT;
	bool			ShouldShowPaths = false;

	// ============ SIMULATION =============
	bool			SimulationBegan = false;
	bool			StartSimulation = false;
	bool			PlaySimulation = false;
	bool			IsSimulationRun = false;
	bool			IsSimulationComplete = false;
	uint32_t		StartIndex;
	ar::mat::Vec4	StartPoint;
	inline void RestartSim(ar::mat::Vec4 p)
	{
		SimulationBegan = false;
		IsSimulationComplete = false;
		StartIndex = 0;
		StartPoint = p;
	}

	// ============ MISC ===================
	float			FPS = 0.0f;
};