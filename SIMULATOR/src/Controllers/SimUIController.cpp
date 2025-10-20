#include "SimUIController.h"
#include <nfd.h>
#include "core/ImGui/PropertyInspector.h"
#include "core/ImGui/ScopedDisable.h"

SimUIController::SimUIController(SimState& state)
	: m_State(state)
{ }

void SimUIController::Render(ar::Ref<ar::Framebuffer> mainFB)
{
	// ddp
	RenderViewport(mainFB);
	RenderLoadPanel();
	RenderMaterialConfigPanel();
	RenderSimulationControlPanel();
	RenderCutterConfigPanel();
	RenderErrorModal();
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

	if (ImGui::Button("Reset"))
		m_State.ShouldReset = true;

	ImGui::End();
}

void SimUIController::RenderSimulationControlPanel()
{
	ar::ScopedDisable disable(m_State.Filepath.empty());
	ImGui::Begin("Simulation");
	ImGui::DragFloat("Speed", &m_State.SimulationSpeed, 1.0f, 1.0f, 100.0f);
	if (ImGui::Button("Start"))
	{
		m_State.StartSimulation = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Instant"))
	{
		m_State.ShouldMillInstant = true;
	}
	if (m_State.IsSimulationRun)
	{
		ImGui::TextWrapped("Simulation currently running...");
	}

	ImGui::End();
}

void SimUIController::RenderCutterConfigPanel()
{
	ar::ScopedDisable disable(m_State.IsSimulationRun);
	ImGui::Begin("Cutter");
	if (!m_State.Filepath.empty())
	{
		ImGui::TextWrapped("Shape: ");
		ImGui::SameLine();
		const char* cutterTypeNames[] = { "Flat", "Round" };
		int currentItem = static_cast<int>(m_State.CutterType);
		if (ImGui::Combo("##cuttertype", &currentItem, cutterTypeNames, IM_ARRAYSIZE(cutterTypeNames)))
		{
			m_State.CutterType = static_cast<CutterType>(currentItem);
		}

		ImGui::TextWrapped("Diameter: ");
		ImGui::SameLine();
		if (ImGui::InputFloat("##diameter", &m_State.CutterSize, 1.0f, 1.0f, "%.3f [mm]"))
		{
			m_State.CutterSize = std::clamp(m_State.CutterSize, 1.0f, 24.0f);
		}
		
		ImGui::TextWrapped("Cutting length: ");
		ImGui::SameLine();
		if (ImGui::InputFloat("##cuttinglength", &m_State.CutterHeight, 1.0f, 1.0f, "%.3f [mm]"))
		{
			m_State.CutterHeight = std::clamp(m_State.CutterHeight, 1.0f, 2 * m_State.CutterSize);
		}
	}
	else
	{
		ImGui::TextWrapped("No file loaded -- cutter information N/A.");
	}
	ImGui::End();
}

void SimUIController::RenderErrorModal()
{
	const char* popupName = "Error";

	if (m_State.ShowErrorModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowErrorModal = false;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::SetWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 40.0f);
		ImGui::TextUnformatted(m_State.ErrorMessages[0].c_str());
		ImGui::PopTextWrapPos();

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Separator();
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) * 0.5f);
		if (ImGui::Button("OK", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
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
