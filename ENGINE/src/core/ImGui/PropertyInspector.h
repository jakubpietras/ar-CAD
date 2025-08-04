#pragma once
#include "core/Core.h"

namespace ar
{
	struct PropertyChangeState 
	{
		bool Changed = false;   
		bool Committed = false;
		bool Activated = false;
	};

	class PropertyInspector
	{
	public:
		static PropertyChangeState InspectProperty(const std::string& label, mat::Vec3& value, mat::Vec3& oldValue, float minValue, float maxValue);
		static PropertyChangeState InspectProperty(const std::string& label, UInt2& value, UInt2& oldValue, uint32_t minValue, uint32_t maxValue);

	private:

	};
}

