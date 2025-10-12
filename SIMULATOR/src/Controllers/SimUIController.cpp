#include "SimUIController.h"
#include <nfd.h>
#include "core/ImGui/PropertyInspector.h"
#include "core/ImGui/ScopedDisable.h"

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
	ImGui::SameLine();
	{
		ar::ScopedDisable disable(m_State.Filepath.empty());
		ImGui::Checkbox("Show Paths", &m_State.ShouldShowPaths);
	}
	ImGui::End();
}

void SimUIController::RenderMaterialConfigPanel()
{
	const size_t minVal = 100, maxVal = 1000;

	ImGui::Begin("Material");
	ar::PropertyInspector::InspectProperty("Divisions", m_State.Material.Samples, minVal, maxVal);
	ar::PropertyInspector::InspectProperty("Size [cm]", m_State.Material.Size, 1, 20, 1.0f);
	ImGui::DragFloat("Base Height [cm]", &m_State.Material.BaseHeight, 1.0f, 1, 20);

	ImGui::End();
}

void SimUIController::RenderSimulationControlPanel()
{
	ImGui::Begin("Simulation");
	ImGui::DragFloat("Speed", &m_State.SimulationSpeed, 1.0f, 1.0f, 10.0f);
	if (ImGui::Button("Start"))
	{
		// todo
	}
	ImGui::SameLine();
	if (ImGui::Button("Instant"))
	{
		// todo
	}

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

		ImGui::DragFloat("Height", &m_State.CutterHeight, 1.0f, 4.0f, 10.0f);
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
