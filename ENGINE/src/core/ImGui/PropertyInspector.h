#pragma once
#include "core/Core.h"
#include "core/Scene/Components.h"
namespace ar
{
	class PropertyInspector
	{
	public:
		static bool InspectProperty(const std::string& label, mat::Vec3& value, mat::Vec3& oldValue, float minValue, float maxValue);
		static bool InspectProperty(const std::string& label, mat::Vec3& value, float minValue, float maxValue, float step = 0.01f);

		static bool InspectProperty(const std::string& label, mat::UInt2& value, mat::UInt2& oldValue, unsigned int minValue, unsigned int maxValue);
		static bool InspectProperty(const std::string& label, mat::UInt2& value, unsigned int minValue, unsigned int maxValue);

		static bool InspectProperty(const std::string& label, float& value, float minValue, float maxValue);
		static bool InspectProperty(const std::string& label, size_t& value, size_t minValue, size_t maxValue);

		static bool InspectProperty(const std::string& label, PivotType& type);

	private:

	};
}

