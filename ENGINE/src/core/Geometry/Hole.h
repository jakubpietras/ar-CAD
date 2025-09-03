#pragma once
#include "core/Geometry/EdgeInfo.h"

namespace ar
{
	struct Hole
	{
		std::array<EdgeInfo, 3> Edges{};
		std::array<uint32_t, 3> Endpoints{};

		Hole(const std::array<EdgeInfo, 3>& edges = {}, const std::array<uint32_t, 3>& endpoints = {})
			: Edges(edges), Endpoints(endpoints) {
		}

		std::string ToString() const {
			return std::to_string(Endpoints[0]) + "-"
				+ std::to_string(Endpoints[1]) + "-"
				+ std::to_string(Endpoints[2]);
		}
	};
}

