#pragma once
#include "core/Core.h"

namespace ar
{
	class PropertyInspector
	{
	public:
		static bool InspectProperty(const std::string& label, mat::Vec3& value, mat::Vec3& oldValue, float minValue, float maxValue);
		static bool InspectProperty(const std::string& label, mat::Vec3& value, float minValue, float maxValue);

		static bool InspectProperty(const std::string& label, UInt2& value, UInt2& oldValue, unsigned int minValue, unsigned int maxValue);
		static bool InspectProperty(const std::string& label, UInt2& value, unsigned int minValue, unsigned int maxValue);

		static bool InspectProperty(const std::string& label, float& value, float minValue, float maxValue);


	private:

	};
}

