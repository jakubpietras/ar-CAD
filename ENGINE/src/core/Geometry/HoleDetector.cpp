#include "arpch.h"
#include "HoleDetector.h"
// #define DEBUG

namespace ar
{

	std::vector<ar::Hole> HoleDetector::Detect(std::vector<Entity> surfaces)
	{
		m_Adjacency.clear();
		m_EdgeTable.clear();

		BuildGraph(surfaces);
#ifdef DEBUG
		PrintAdjacency();
		PrintEdgeTable();
#endif
		// triples of points that (in order) form a triangular hole
		return ConvertTriplesToHoles(FindTriples());
	}

	std::unordered_map<uint32_t, std::unordered_set<uint32_t>> HoleDetector::m_Adjacency;
	std::unordered_map<EdgeKey, EdgeInfo, EdgeKeyHash> HoleDetector::m_EdgeTable;

	void HoleDetector::BuildGraph(std::vector<Entity> surfaces)
	{
		for (auto& surface : surfaces)
			ExtractEdges(surface);
	}

	std::vector<std::array<uint32_t, 3>> HoleDetector::FindTriples()
	{
		std::vector<std::array<uint32_t, 3>> triples;

		// starting from every corner point (endpoint)
		for (auto& [point, neighbors] : m_Adjacency)
		{
			// iterate over its neighbors
			for (auto& neighbor : neighbors)
			{
				for (auto& secondNeighbor : m_Adjacency[neighbor])
				{
					// if neighbor's neighbor is the starting point's neighbor
					if (m_Adjacency[secondNeighbor].contains(point))
					{
						// add the triple
						triples.push_back({ point, neighbor, secondNeighbor });
					}
				}
			}
		}

		for (auto& triple : triples)
			std::ranges::sort(triple);

		std::ranges::sort(triples);
		auto last = std::unique(triples.begin(), triples.end());
		triples.erase(last, triples.end());

		return triples;
	}

	void HoleDetector::ExtractEdges(Entity surface)
	{
		auto& s = surface.GetComponent<ar::SurfaceComponent>();
		auto& points = surface.GetComponent<ar::ControlPointsComponent>().Points;
		uint32_t firstPointID = 0, secondPointID = 0;

		for (size_t sv = 0; sv < s.Description.Segments.v; sv++)
			for (size_t su = 0; su < s.Description.Segments.u; su++)
			{
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::BOTTOM, su, sv), surface);
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::RIGHT, su, sv), surface);
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::TOP, su, sv), surface);
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::LEFT, su, sv), surface);
			}
	}

	EdgeInfo HoleDetector::ExtractEdge(Entity surface, EdgeInfo::Placement placement, uint32_t segmentU, uint32_t segmentV)
	{
		auto& size = surface.GetComponent<ar::SurfaceComponent>().Description.Size;
		auto getIndex = [&size](int u, int v) -> uint32_t {
			return v * size.u + u;
		};

		size_t		baseV = 3 * segmentV,
					baseU = 3 * segmentU;
		uint32_t	firstIndex = 0,
					secondIndex = 0;
	
		switch (placement)
		{
			case EdgeInfo::Placement::BOTTOM:
			{
				firstIndex = getIndex(baseU + 0, baseV + 0);
				secondIndex = getIndex(baseU + 3, baseV + 0);
				break;
			}
			case EdgeInfo::Placement::RIGHT:
			{
				firstIndex = getIndex(baseU + 3, baseV + 0);
				secondIndex = getIndex(baseU + 3, baseV + 3);
				break;
			}
			case EdgeInfo::Placement::TOP:
			{
				firstIndex = getIndex(baseU + 3, baseV + 3);
				secondIndex = getIndex(baseU + 0, baseV + 3);
				break;
			}
		
			case EdgeInfo::Placement::LEFT:
			{
				firstIndex = getIndex(baseU + 0, baseV + 3);
				secondIndex = getIndex(baseU + 0, baseV + 0);
				break;
			}
		}
		return { placement, EdgeKey(firstIndex, secondIndex), surface };
	}

	void HoleDetector::ProcessEdge(EdgeInfo edge, Entity surface)
	{
		auto& points = surface.GetComponent<ar::ControlPointsComponent>().Points;
		uint32_t firstEndpointID = points[edge.GridPlacement.first].GetID();
		uint32_t secondEndpointID = points[edge.GridPlacement.second].GetID();
		
		// Both endpoints the same - edge collapsed to a single point
		if (firstEndpointID == secondEndpointID)
			return;

		// Adjacency info (only point connectivity)
		m_Adjacency[firstEndpointID].insert(secondEndpointID);
		m_Adjacency[secondEndpointID].insert(firstEndpointID);
		
		// Edge info (full info for reproduction)
		m_EdgeTable[EdgeKey(firstEndpointID, secondEndpointID)] = edge;	
		EdgeInfo edgeReverse(edge);
		std::swap(edgeReverse.GridPlacement.first, edgeReverse.GridPlacement.second);
		m_EdgeTable[EdgeKey(secondEndpointID, firstEndpointID)] = edgeReverse;
	}

	std::vector<ar::Hole> HoleDetector::ConvertTriplesToHoles(std::vector<std::array<uint32_t, 3>> triples)
	{
		std::vector<ar::Hole> holes;
		for (const auto& triple : triples)
		{
			Hole h({
				m_EdgeTable[EdgeKey(triple[0], triple[1])],
				m_EdgeTable[EdgeKey(triple[1], triple[2])],
				m_EdgeTable[EdgeKey(triple[2], triple[0])]
				}, triple);

			holes.push_back(std::move(h));
		}
		return holes;
	}

	void HoleDetector::PrintAdjacency()
	{
		std::string logMessage;
		for (auto& entry : m_Adjacency)
		{
			logMessage += std::to_string(entry.first) + ": {";
			for (auto& ref : entry.second)
				logMessage += std::to_string(ref) + " ";
			logMessage += "}\n";
		}
		AR_TRACE(logMessage);
	}

	void HoleDetector::PrintEdgeTable()
	{
		std::string logMessage;
		for (auto& entry : m_EdgeTable)
		{
			logMessage += "[(" + std::to_string(entry.first.first) + ", " + std::to_string(entry.first.second) + ")]: "
				+ "patch " + std::to_string(entry.second.Patch.GetID()) + ", grid index (a="
				+ std::to_string(entry.second.GridPlacement.first) + ", b=" + std::to_string(entry.second.GridPlacement.second) + ")\n";
		}
		AR_TRACE(logMessage);
	}

}