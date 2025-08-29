#pragma once
#include <unordered_map>
#include <unordered_set>
#include <utility>
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
		Entity Patch;
	};

	struct Hole
	{
		std::array<EdgeInfo, 3> Edges;
		std::array<uint32_t, 3> Endpoints;

		Hole(const std::array<EdgeInfo, 3>& edges = {}, const std::array<uint32_t, 3>& endpoints = {})
			: Edges(edges), Endpoints(endpoints) {
		}

		std::string ToString() const {
			return std::to_string(Endpoints[0]) + "-"
				+ std::to_string(Endpoints[1]) + "-"
				+ std::to_string(Endpoints[2]);
		}
	};

	class HoleDetector
	{
	public:
		static std::vector<Hole> Detect(std::vector<Entity> surfaces);

	private:
		// For quick lookup of adjacency
		static std::unordered_map<uint32_t, std::unordered_set<uint32_t>> m_Adjacency;
		
		// For retrieving edge information based on the endpoints
		static std::unordered_map<EdgeKey, EdgeInfo, EdgeKeyHash> m_EdgeTable;
	
		static void BuildGraph(std::vector<Entity> surfaces);
		static std::vector<std::array<uint32_t, 3>> FindTriples();

		static void ExtractEdges(Entity surface);
		static EdgeInfo ExtractEdge(Entity surface, EdgeInfo::Placement placement, uint32_t segmentU, uint32_t segmentV);
		static void ProcessEdge(EdgeInfo edge, Entity surface);
		static std::vector<ar::Hole> ConvertTriplesToHoles(std::vector<std::array<uint32_t, 3>> triples);

		// Debug
		static void PrintAdjacency();
		static void PrintEdgeTable();
	};
}

