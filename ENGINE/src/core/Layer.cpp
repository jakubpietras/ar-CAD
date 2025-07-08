#include "arpch.h"
#include "Layer.h"

namespace ar
{
	Layer::Layer(const std::string& name)
		: m_DebugName(name)
	{ }

	Layer::~Layer()
	{
	}
}