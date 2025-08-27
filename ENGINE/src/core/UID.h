#pragma once
#include <string>
#include <unordered_set>

namespace ar
{
	class UID
	{
	public:
		static const uint32_t AssignNew() { return NextAvailable(); }
		static const void AddUsed(uint32_t id) { m_UsedIDs.emplace(id); }
	private:
		static uint32_t NextAvailable();
		static std::unordered_set<uint32_t> m_UsedIDs;
	};
}
