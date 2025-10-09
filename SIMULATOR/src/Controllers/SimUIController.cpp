#include "SimUIController.h"
#include <nfd.h>

SimUIController::SimUIController(SimState& state)
	: m_State(state)
{ }

void SimUIController::Render(ar::Ref<ar::Framebuffer> mainFB)
{
	RenderViewport(mainFB);
	RenderLoadPanel();
	RenderMaterialConfigPanel();
	RenderSimulationControlPanel();
	RenderCutterConfigPanel();
}

void SimUIController::RenderViewport(ar::Ref<ar::Framebuffer> mainFB)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	ar::Application::Get().ImGuiBlockEvents(!ImGui::IsWindowHovered());

	auto viewportSize = ImGui::GetContentRegionAvail();

	if (viewportSize.x != m_State.Viewport.Width || viewportSize.y != m_State.Viewport.Height)
	{
		m_State.Viewport = { viewportSize.x, viewportSize.y };
		m_State.ViewportResized = true;
	}

	ImGui::Image(
		(ImTextureID)(uintptr_t)mainFB->GetColorAttachment(),
		viewportSize,
		ImVec2(0, 1), ImVec2(1, 0)
	);

	ImGui::End();
	ImGui::PopStyleVar();
}

void SimUIController::RenderLoadPanel()
{
	ImGui::Begin("Path Loading");
	std::string msg = "Currently loaded file: ";
	if (m_State.Filepath.empty())
		msg = "No file loaded!";
	else
		msg += m_State.Filepath.filename().string();

	ImGui::TextWrapped(msg.c_str());
	if (ImGui::Button("Load"))
		OpenImportDialog();
	ImGui::End();
}

void SimUIController::RenderMaterialConfigPanel()
{
	ImGui::Begin("Material");
	ImGui::Text("Material config window");
	ImGui::End();
}

void SimUIController::RenderSimulationControlPanel()
{
	ImGui::Begin("Simulation");
	ImGui::Checkbox("Show Paths", &m_State.ShouldShowPaths);

	ImGui::Text("Simulation control panel");
	ImGui::End();
}

void SimUIController::RenderCutterConfigPanel()
{
	ImGui::Begin("Cutter");
	if (!m_State.Filepath.empty())
	{
		std::string type = "Type: ";
		if (m_State.CutterType == CutterType::FLAT)
			type += "Flat";
		else
			type += "Round";
		ImGui::TextWrapped(type.c_str());

		std::string trimmedNumber = 
			std::to_string(m_State.CutterSize)
			.substr(0, std::to_string(m_State.CutterSize).find('.') + 3);
		std::string size = "Size: " + trimmedNumber + " [mm]";
		ImGui::TextWrapped(size.c_str());
	}
	else
	{
		ImGui::TextWrapped("No file loaded -- cutter information N/A.");
	}
	ImGui::End();
}

void SimUIController::OpenImportDialog()
{
	auto path = OpenFileDialog();
	if (!path.empty())
	{
		m_State.Filepath = path;
		m_State.ShouldImport = true;
	}
	else
		AR_TRACE("Import canceled");
}

fs::path SimUIController::OpenFileDialog()
{
	std::string path;
	nfdu8char_t* outPath;
	nfdu8filteritem_t filters[1] = { { "G-code", "k*,f*" } };
	nfdopendialogu8args_t args = { 0 };
	args.filterList = filters;
	args.filterCount = 1;
	nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
	if (result == NFD_OKAY)
	{
		path = std::string(static_cast<char*>(outPath));
		AR_TRACE("File {0} opened sucessfully", path);
		NFD_FreePathU8(outPath);
	}
	else if (result == NFD_CANCEL)
	{
		AR_TRACE("File dialog closed (action canceled)");
	}
	else
	{
		AR_ERROR("Error opening file: {0}", NFD_GetError());
	}
	return fs::path(path);
}
