#pragma once
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "core/Scene/Entity.h"
#include "core/Geometry/EdgeInfo.h"
#include "core/Geometry/Hole.h"

namespace ar
{
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

