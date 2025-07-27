#include "arpch.h"
#include "UID.h"

namespace ar
{
	std::unordered_set<uint32_t> UID::m_UsedIDs{};

	uint32_t UID::NextAvailable()
	{
		static int counter = 1;
		while (m_UsedIDs.find(counter) != m_UsedIDs.end())
			counter++;
		m_UsedIDs.insert(counter);
		return counter;
	}
}

