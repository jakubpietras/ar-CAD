#include "arpch.h"
#include "PropertyInspector.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace ar
{
	bool PropertyInspector::InspectProperty(const std::string& label, mat::Vec3& value, mat::Vec3& oldValue, float minValue, float maxValue)
	{
		oldValue = value;
		return ImGui::DragFloat3(label.c_str(), value.Data(), 0.01f, minValue, maxValue);
	}

	bool PropertyInspector::InspectProperty(const std::string& label, mat::UInt2& value, mat::UInt2& oldValue, unsigned int minValue, unsigned int maxValue)
	{
		oldValue = value;

		int temp[2] = {
			static_cast<int>(value.u),
			static_cast<int>(value.v)
		};
		bool changed = ImGui::DragInt2(label.c_str(), temp, 0.01f, static_cast<int>(minValue), static_cast<int>(maxValue));
		if (changed) {
			value.u = static_cast<uint32_t>(temp[0]);
			value.v = static_cast<uint32_t>(temp[1]);
		}

		return changed;
	}

	bool PropertyInspector::InspectProperty(const std::string& label, float& value, float minValue, float maxValue)
	{
		return ImGui::DragFloat(label.c_str(), &value, 0.01f, minValue, maxValue);
	}

	bool PropertyInspector::InspectProperty(const std::string& label, mat::Vec3& value, float minValue, float maxValue)
	{
		return ImGui::DragFloat3(label.c_str(), value.Data(), 0.01f, minValue, maxValue);
	}

	bool PropertyInspector::InspectProperty(const std::string& label, mat::UInt2& value, unsigned int minValue, unsigned int maxValue)
	{
		int temp[2] = {
			static_cast<int>(value.u),
			static_cast<int>(value.v)
		};
		bool changed = ImGui::DragInt2(label.c_str(), temp, 1.0f, static_cast<int>(minValue), static_cast<int>(maxValue));
		if (changed) {
			value.u = static_cast<uint32_t>(temp[0]);
			value.v = static_cast<uint32_t>(temp[1]);
		}

		return changed;
	}

	bool PropertyInspector::InspectProperty(const std::string& label, PivotType& type)
	{
		auto current = static_cast<int>(type);
		static const char* options[] = {"Local origin", "Cursor"};
		bool changed = ImGui::Combo(label.c_str(), &current, options, IM_ARRAYSIZE(options));
		if (changed)
			type = static_cast<PivotType>(current);
		return changed;
	}

	bool PropertyInspector::InspectProperty(const std::string& label, size_t& value, size_t minValue, size_t maxValue)
	{
		return ImGui::DragScalar(label.c_str(), ImGuiDataType_U64, &value, 1.0f, &minValue, &maxValue);
	}

}