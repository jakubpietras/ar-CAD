#pragma once
#include <imgui.h>
#include <ARCAD.h>
#include "SimState.h"

namespace fs = std::filesystem;

class SimUIController
{
public:
	SimUIController(SimState& state);
	void Render(ar::Ref<ar::Framebuffer> mainFB);
private:
	SimState& m_State;
	void RenderViewport(ar::Ref<ar::Framebuffer> mainFB);
	void RenderLoadPanel();
	void RenderMaterialConfigPanel();
	void RenderSimulationControlPanel();
	void RenderCutterConfigPanel();
	void RenderErrorModal();

	void OpenImportDialog();
	fs::path OpenFileDialog();
};