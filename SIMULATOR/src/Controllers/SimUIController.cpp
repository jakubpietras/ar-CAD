#include "SimUIController.h"

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
	ImGui::Text("Path loading window");
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
	ImGui::Text("Simulation control panel");
	ImGui::End();
}

void SimUIController::RenderCutterConfigPanel()
{
	ImGui::Begin("Cutter");
	ImGui::Text("Cutter config panel");
	ImGui::End();
}
