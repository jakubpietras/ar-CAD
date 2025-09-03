#pragma once
#include "core/Scene/Entity.h"

namespace ar
{
	using EdgeKey = std::pair<uint32_t, uint32_t>;
	struct EdgeKeyHash {
		size_t operator()(const EdgeKey& p) const noexcept {
			uint64_t combined = (static_cast<uint64_t>(p.first) << 32) | p.second;
			return std::hash<uint64_t>{}(combined);
		}
	};
	struct EdgeInfo
	{
		enum class Placement { TOP, BOTTOM, LEFT, RIGHT };
		Placement EdgePlacement;
		std::pair<uint32_t, uint32_t> GridPlacement;

		std::vector<ar::Entity> Points;
		std::vector<ar::Entity> Neighbors;

		Entity Patch;
	};
}

