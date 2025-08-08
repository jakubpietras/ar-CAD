#include "ImGuiUtils.h"
#include <imgui.h>

void CenterNextPopup()
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
}
