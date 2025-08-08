#include "arpch.h"
#include "PropertyInspector.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace ar
{
	bool PropertyInspector::InspectProperty(const std::string& label, mat::Vec3& value, mat::Vec3& oldValue, float minValue, float maxValue)
	{
		oldValue = value;
		return ImGui::DragFloat3(label.c_str(), value.Data(), 0.1f, minValue, maxValue);
	}

	bool PropertyInspector::InspectProperty(const std::string& label, UInt2& value, UInt2& oldValue, unsigned int minValue, unsigned int maxValue)
	{
		oldValue = value;

		int temp[2] = {
			static_cast<int>(value.u),
			static_cast<int>(value.v)
		};
		bool changed = ImGui::DragInt2(label.c_str(), temp, 0.1f, static_cast<int>(minValue), static_cast<int>(maxValue));
		if (changed) {
			value.u = static_cast<uint32_t>(temp[0]);
			value.v = static_cast<uint32_t>(temp[1]);
		}

		return changed;
	}

	bool PropertyInspector::InspectProperty(const std::string& label, float& value, float minValue, float maxValue)
	{
		return ImGui::DragFloat(label.c_str(), &value, 0.1f, minValue, maxValue);
	}

	bool PropertyInspector::InspectProperty(const std::string& label, mat::Vec3& value, float minValue, float maxValue)
	{
		return ImGui::DragFloat3(label.c_str(), value.Data(), 0.1f, minValue, maxValue);

	}

	bool PropertyInspector::InspectProperty(const std::string& label, UInt2& value, unsigned int minValue, unsigned int maxValue)
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

}