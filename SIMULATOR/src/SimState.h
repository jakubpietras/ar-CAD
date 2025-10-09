#pragma once
#include <vector>
#include <string>

struct ViewportSize
{
	float Width = 0, Height = 0;
};

class SimState
{
public:
	// ============ VIEWPORT ==============
	ViewportSize Viewport{ 0.f, 0.f };
	bool ViewportResized = false;

	// ============ ERRORS ================
	std::vector<std::string> ErrorMessages;
	bool ShowErrorModal = false;
	inline void ClearErrorState()
	{
		ErrorMessages.clear();
		ShowErrorModal = false;
	}

	// ============ LOADING ===============
	std::string Filepath;
	bool ShouldImport = false;
};