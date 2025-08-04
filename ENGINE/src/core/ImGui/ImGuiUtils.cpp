#include "arpch.h"
#include "ImGuiUtils.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace ar
{
	bool IsItemActivePreviousFrame()
	{
		ImGuiContext& g = *GImGui;
		if (g.ActiveIdPreviousFrame)
			return g.ActiveIdPreviousFrame == GImGui->LastItemData.ID;
		return false;
	}
}