#include "arpch.h"
#include "PropertyInspector.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "ImGuiUtils.h"

namespace ar
{
	PropertyChangeState PropertyInspector::InspectProperty(const std::string& label, mat::Vec3& value, mat::Vec3& oldValue, float minValue, float maxValue)
	{
		PropertyChangeState changeState{};

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
		const char* labels[3] = { "X", "Y", "Z" };
		ImVec4 colors[3] = {
			{ 0.8f, 0.1f, 0.15f, 1.0f }, // Red
			{ 0.2f, 0.7f, 0.2f, 1.0f }, // Green
			{ 0.1f, 0.25f, 0.8f, 1.0f } // Blue
		};
		float* components[3] = { &value.x, &value.y, &value.z };
		for (int i = 0; i < 3; ++i)
		{
			ImVec4 baseColor = colors[i];
			ImVec4 hoveredColor = ImVec4(
				std::min(baseColor.x + 0.1f, 1.0f),
				std::min(baseColor.y + 0.1f, 1.0f),
				std::min(baseColor.z + 0.1f, 1.0f),
				baseColor.w
			);
			ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, baseColor);
			if (ImGui::Button(labels[i], buttonSize))
			{
				*components[i] = 0.0f;
				changeState.Committed = true;
			}
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			std::string dragLabel = "##" + std::string(labels[i]);
			bool edited = ImGui::DragFloat(dragLabel.c_str(), components[i], 0.1f, minValue, maxValue, "%.2f");
			if (edited)
				changeState.Changed = true;
			if (ImGui::IsItemActivated())
				changeState.Activated = true;
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				changeState.Committed = true;
			}
			ImGui::PopItemWidth();
			if (i < 2)
				ImGui::SameLine();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
		return changeState;
	}

	PropertyChangeState PropertyInspector::InspectProperty(const std::string& label, UInt2& value, UInt2& oldValue, uint32_t minValue, uint32_t maxValue)
	{
		PropertyChangeState changeState{};
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
		const char* labels[2] = { "U", "V" };
		ImVec4 colors[2] = {
			{ 0.8f, 0.1f, 0.15f, 1.0f }, // Red
			{ 0.2f, 0.7f, 0.2f, 1.0f }   // Green
		};
		uint32_t* components[2] = { &value.u, &value.v };
		for (int i = 0; i < 2; ++i)
		{
			ImVec4 baseColor = colors[i];
			ImVec4 hoveredColor = ImVec4(
				std::min(baseColor.x + 0.1f, 1.0f),
				std::min(baseColor.y + 0.1f, 1.0f),
				std::min(baseColor.z + 0.1f, 1.0f),
				baseColor.w
			);
			ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, baseColor);
			if (ImGui::Button(labels[i], buttonSize))
			{
				*components[i] = 0;
				changeState.Committed = true;
			}
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			std::string dragLabel = "##" + std::string(labels[i]);

			int intValue = static_cast<int>(*components[i]);
			bool edited = ImGui::DragInt(dragLabel.c_str(), &intValue, 1.0f, static_cast<int>(minValue), static_cast<int>(maxValue));
			if (edited)
			{
				*components[i] = static_cast<uint32_t>(std::max(0, intValue));
				changeState.Changed = true;
			}

			if (ImGui::IsItemActivated())
				changeState.Activated = true;
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				changeState.Committed = true;
			}
			ImGui::PopItemWidth();
			if (i < 1)
				ImGui::SameLine();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
		return changeState;
	}

}